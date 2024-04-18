#!/bin/bash

SPELLLIST="https://raw.githubusercontent.com/astranauta/astranauta.github.io/master/data/spells.json"

curl $SPELLLIST
  | jq -r ".spell | map(.name)| .[]" \
  | tr "[:upper:]" "[:lower:]" \
  > spellnames.txt
