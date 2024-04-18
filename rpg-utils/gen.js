import _ from 'lodash';
import program from 'commander';
import {
  getRandomInt,
  choose,
  chooseWeighted,
  isWeighted,
  getDetails,
  generateConsoleDescriptionFromTemplate
} from './utils';

import sentientItem from './generators/SentientItem';
import artifact from './generators/Artifact';
import diaspora from './generators/Diaspora';

let generators = {
  'sentient-item': {description: "Generate a sentient Item", generator: sentientItem},
  artifact: {description: "Generate an artifact", generator: artifact },
  'diaspora-system': {description:'Generate a Diaspora system', generator: diaspora}
}

function main(){
  let generator, quantity;

  program
    .version('0.0.1')
    .arguments('<generator> [quantity]')
    .action((gen, num)=>{
      generator = gen;
      quantity = num;
    })
    .parse(process.argv)

  if(!(generator in generators)){
    console.log("You must choose a generator from one of the below:\n")
    _.each(generators, (v,k)=>console.log(`${k}: ${v["description"]}`));
  }else{
    if(_.isArray(generators[generator]["generator"])){
      generateConsoleDescriptionFromTemplate(generators[generator]["description"], generators[generator]["generator"])
    }else if (_.isFunction(generators[generator]["generator"])){
      generators[generator]["generator"]();
    }
  }

}

main();
