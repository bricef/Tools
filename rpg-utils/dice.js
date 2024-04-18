var _ = require("lodash");

/*
 * Basic dice setup
 * 
 * Dice are represented as discrete probability distribution.
 * This is represented as a JS object where the keys are the 
 * possible results and the values are the probablity for that
 * result. For example, a d4:
 *
 * { 1: 0.25, 2: 0.25, 3: 0.25, 4: 0.25 }  
 *
 * While key ordering seems stable, this shouldn't be relied 
 * upon.
 */

function mkdice(N){
  // The mkdice function creates a fair N-sided dice.
  var dice = {};
  for (var i = 1; i <= N; i++){
    dice[i] = 1.0/N;
  }
  return dice;
}

var coin = {head: 0.5, tail: 0.5}
var d4 = mkdice(4);
var d6 = mkdice(6);
var d8 = mkdice(8);
var d10 = mkdice(10);
var d12 = mkdice(12);
var d20 = mkdice(20);
var d100 = mkdice(100);
var dF = {[0]: 2/6, [+1]: 2/6, [-1]: 2/6}

function roll(dice){
  var t = Math.random();
  // Note that we can't assume a fair dice, so we 
  // have to go through a proper proportional
  // selection instead of picking at random.
  return _.chain(discrete2cumulative(dice))
    .dropWhile((pv) => pv[0] < t)
    .first()
    .value()[1];
}

/*
 * Utility functions
 */
function discrete2cumulative(dist){
  return _.chain(dist)
    .reduce((cumulative, prob, v)=>{
      var current_total = _.last(cumulative)[0];
      cumulative.push([current_total+prob, v])
      return cumulative;
    }, [[0.0, null]])
    .drop()
    .value();
}

function probability_product(fn){
  var f = function add(a, b){
    var newdice = {};
    _.forEach(a, (va, ka)=>{
      _.forEach(b, (vb, kb)=>{
        var key = fn(parseInt(ka), parseInt(kb));
        var prob = va*vb;
        if (key in newdice){
          newdice[key] = newdice[key]+prob;
        }else{
          newdice[key] = prob;
        }
      })
    }) 
    return newdice;
  };
  return f;
}

function p_of(dist, predicate){
  var p = (
    _.chain(dist)
      .filter(predicate)
      .map((prob,balance)=>prob)
      .reduce((p1,p2)=>p1+p2)
      .value()
  );
  return p ? p : 0.0;
}

function p_geq(dc, dice){
  var sum = 0.0;
  _.forEach(dice, (prob,roll)=>{
    if(roll >= dc){
      sum += prob; 
    }
  })
  return sum;
}

function modify(v, fns){
  return _.reduce(fns, (v, fn)=>{
    return fn(v)
  },v)
}

function redice(map, dice){
  // eg {1: "X", 2:"X", 3:"Y", 4:"Y"}, d4  => {"X": 0.5, "Y":0.5}
  var newdice = {}
  _.forEach(dice, (v,k)=>{
    var newkey = (k in map)? map[k] : k;
    if(newkey in newdice){
      newdice[newkey] += dice[k];
    }else{
      newdice[newkey] = dice[k];
    }
  })
  return newdice;  
}


/*
 * General dice functions
 * Note that these assume a numeric dice
 */
var add = probability_product((a,b)=>a+b);
var lowest = probability_product(Math.min);
var highest = probability_product(Math.max);
var add_constant = function(constant, dice){
  return _.mapKeys(dice, (prob, roll) => parseInt(roll) + constant);
}
var plus = (d) => _.curry(add)(d);
var identity = (x) => x;

/*
 * Game verbs
 */
var advantage = (d)=>highest(d,d);
var disadvantage = (d)=>lowest(d,d);
var guided = (d)=>add(d, d4);
var with_bonus = (bonus) => _.curry(add_constant)(bonus)
var aided = advantage;
var inspired = plus;

/*
 * Simulation of check series
 */
function p_success(DC, modifiers){
  var dice = modify(d20, modifiers);
  var p_success = p_geq(DC, dice);
  return p_success;
}

function p_succeed_all(DC, modifiers, num_checks){
  return Math.pow(p_success(DC, modifiers), num_checks);
}

function p_succeed_over_half(DC, modifiers, num_checks){
  return p_succeed_all(DC, modifiers, Math.ceil(num_checks/2.0));
}

function p_outcomes(DC, is_aided, is_guided, inspire_dice, bonus, num_checks){
  var modifiers = [
    is_guided ? guided : identity,
    inspire_dice ? inspired(inspire_dice) : identity,
    bonus ? with_bonus(bonus) : identity
  ];

  var base_dice = is_aided ? aided(d20) : d20;
  var p_critical_fail = base_dice[1];
  var p_critical_success = base_dice[20];

  // console.log("base_dice", base_dice) 
  // console.log("p_critical_fail", p_critical_fail) 
  // console.log("p_critical_success", p_critical_success) 

  // Critical success and critial failure probabilities are fixed.
  // We still need critical success probability, because a critical success 
  // might not be enought to beat the DC!
  
  var critless_dice = _.omit(base_dice, [1, 20]);p_critical_success
  var roll_dice = modify(critless_dice, modifiers);
  var p_succeed = p_geq(DC, roll_dice);
  var p_fail = p_of(roll_dice, (prob, roll) => parseInt(roll)<DC );
  
  // console.log("critless_dice", critless_dice) 
  // console.log("roll_dice", roll_dice); 
  // console.log("p_succeed", p_succeed);
  // console.log("p_fail", p_fail);
  

  var smartdice = {
    [-2]: p_critical_fail,
    [-1]: p_fail,
    [1]: p_succeed + p_critical_success
  }
  // console.log(smartdice)
  
  // failures >= 1/2 num rolls => failed.
  // Implies balance of cumulative roll on smart dice should be >0

  // all possible outcomes
  var outcomes = Array(num_checks).fill(smartdice).reduce(add);
  
  // Outcomes that fail
  var p_balance_fail = p_of(outcomes, (prob,balance) => (balance <= 0) );
  var p_balance_success = p_of(outcomes, (prob,balance) => (balance > 0) );

  return {
    p_fail: p_balance_fail,// the crafting fails
    p_succeed: p_balance_success,// the crafting succeeds
    p_critical_success: Math.pow(p_critical_success, Math.ceil(num_checks/2.0)) // over half rolled are 20s
  }
  
}

/*
 * Example
 */
function show_trial(DC, aided, guided, inspiration_dice, bonus, number){
  console.log("CRITICAL RULES: DC"+DC+", "+number+" trials"+(aided? ", aided": "")+(guided? ", guided": "")+(inspiration_dice?", inspired":"")+", +"+bonus+" mod");
  console.log(p_outcomes(DC, aided, guided, inspiration_dice, bonus, number));
  console.log("");
}


if (typeof module != 'undefined' &&!module.parent) {
  // Run a d20 contest with 10 checks agains a DC of 15
  console.log("SIMPLE RULES: DC23, 48 trials, aid, guidance, d10 inspiration and +10 mod");
  console.log(
    p_succeed_all(23, [aided, with_bonus(10), guided, inspired(d10)], 10)
  );
  console.log("");

  console.log("Rolling 4dF");
  console.log(
    [dF, dF, dF, dF].reduce(add)
  );
  console.log("");

  show_trial(23, true, true, d10, 10, 48);
  show_trial(23, false, false, false, 10, 48)
  
  console.log("Ring of plane shift, once per day.");
  show_trial(16, false, false, false, 10, 8)

  console.log("Ring of plane shift, twice per day.");
  show_trial(17, true, false, false, 10, 16)
  
  console.log("Ring of shield once per day.");
  show_trial(16, false, false, false, 10, 1)

  console.log("Crafting level 9 scroll according to Brice's rules.");
  // show_trial(19, false, false, false, 10, 82)
  // Run a d20 contest with 10 checks agains a DC of 15
  console.log("SIMPLE RULES: DC19, aided, guided, inspired, 82 trials and +10 mod");
  console.log(
    p_succeed_all(19, [aided, guided, inspired(d10), with_bonus(10)], 82)
  );
  console.log("");


  console.log("Ring of Greater Invisibility twice a day.");
  show_trial(17, false, false, false, 10, 13)

  console.log("Ring of Greater Invisibility once a day.");
  show_trial(16, false, false, false, 10, 9)
}
