import fs from 'fs';

import {choose} from '../utils';

require.extensions['.txt'] = function (module, filename) {
    module.exports = fs.readFileSync(filename, 'utf8');
};

let names = {
  aboriginal:{
    m: require("./namelists/aboriginal-male.txt").split("\n"),
    f: require("./namelists/aboriginal-female.txt").split("\n"),
  },
  akkadian:{
    m: require("./namelists/akkadian-male.txt").split("\n"),
    f: require("./namelists/akkadian-female.txt").split("\n"),
  }
}

export default () => choose(names.akkadian.m)
