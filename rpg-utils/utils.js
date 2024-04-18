import _ from 'lodash';
import wrap from 'word-wrap';

export function getRandomInt(min, max) {
  return Math.floor(Math.random() * (max - min)) + min;
}

export function choose(choices){
    return choices[Math.floor(Math.random()*choices.length)];
}

export function chooseWeighted(weightedChoices){
  let max = _.last(weightedChoices)[0];
  let roll = getRandomInt(0,max);
  return _.find(weightedChoices, (v)=>roll<=v[0])[1];
}

let isWeighted = (choices) => _.isArray(choices[0]);

export function getDetails(template){
  return _.chain(template).map((choices)=>{
    if(isWeighted(choices)){ return chooseWeighted(choices); }
    else{ return choose(choices); }
  }).value()
}

export function generateConsoleDescriptionFromTemplate(name, template){
  console.log(`${name}`);
  console.log("=".repeat(50))
  getDetails(template).map((desc)=>"• "+desc).forEach((description)=>{
    console.log(wrap(description, {indent:""}));
  })
}

export function fudgeDice(){
  return choose([1,1,0,0,-1,-1]);
}

export function fudgeRoll(){
  return _.sum(_.times(4,fudgeDice));
}
