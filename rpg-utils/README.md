# Random Generators

## Dice utilities 

    ➤ node dice.js 


## Generator: Diaspora System

This generator uses [Diaspora's](http://www.vsca.ca/Diaspora/) system generation rules to generate a system layout. Blue numbers are for the technology level, green for the environment, and red fro resources.

The generator will output a valid [DOT language](https://en.wikipedia.org/wiki/DOT_(graph_description_language)) file. This can be turned into an image if you have graphviz installed. On mac, you can install it with `➤ brew install graphviz`

    ➤ babel-node gen diaspora-system | tee /dev/tty | dot -Tpng -o test.png && open test.png
