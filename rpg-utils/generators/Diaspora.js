import _ from 'lodash';

import Set from 'collections/set';

import {fudgeRoll} from '../utils';
import genName from './Names';

let alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

function genSystem(){
  return { technology: fudgeRoll(), environment: fudgeRoll(), resources: fudgeRoll() }
}

function allNodes(graph){
  return _.reduce(graph, (acc,v,i)=>{
    acc.add(v[0]);
    acc.add(v[1]);
    return acc;
  }, new Set())
}

function nextUnconnected(graph, systems){
  let nodeSet = allNodes(graph);
  let nextNode = _.find(systems, (sys)=>!nodeSet.has(sys.id))
  return (nextNode)?nextNode.id:null;

}

function makeGraph(systems){
  let graph = []
  systems.forEach((system, n, systems)=>{
    if(n==systems.length-1) { return null; } // ignore last item

    // Conect to neighbour
    graph.push([system.id, systems[n+1].id])

    let test = fudgeRoll()

    if(test < 0) { return null; }//we're done

    let next_id = nextUnconnected(graph,systems)
    if(next_id){
      graph.push([system.id, next_id])
    }

    if(test == 0) { return null; }//we're done

    next_id = nextUnconnected(graph,systems)
    if(next_id){
      graph.push([system.id, next_id])
    }
  })
  return graph;
}

function systemsToConsoleString(systems){
  return _(systems).map((sys)=>{
    return `${sys.id}:\n  Technology:${sys.technology}\n  Environment:${sys.environment}\n  Resources:${sys.resources}`
  }).join("\n")
}

function systems2IndexedSystems(systems){
  return systems.reduce((acc,sys)=>{
    return _.assignIn({}, acc, {[sys.id]: sys})
  }, {})
}

function graph2dot(graph, indexedSystems, name="systemlayout"){
  return `graph ${name} {\n` +
    _(indexedSystems).map((sys, name) => {
      return `  "${name}" [label= < <FONT POINT-SIZE="15"><B>${name}</B> <BR/> <FONT COLOR="blue">${sys.technology}</FONT> <FONT COLOR="#008000">${sys.environment}</FONT> <FONT COLOR="red">${sys.resources}</FONT></FONT> >]`
    }).join(";\n") +
    "\n" +
    _(graph).map((link) => `  "${link[0]}" -- "${link[1]}"`).join(";\n") +
    "\n} ";
}



export default () => {
  let num_systems = 8;
  let names = _.take(_.uniq(_.times(2*num_systems, genName)), num_systems)
  let stats = _.times(num_systems, genSystem);
  let namedSystems = _.map(stats, (s,k) => _.assign({}, s,{id:names[k]}));
  let graph = makeGraph(namedSystems)
  console.log("/*")
  console.log(systemsToConsoleString(namedSystems))
  console.log("*/")
  console.log(graph2dot(graph, systems2IndexedSystems(namedSystems)))
  // console.log(JSON.stringify(systems, null, "  "));
}
