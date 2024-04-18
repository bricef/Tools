#!/bin/bash

DICELIST_DIR="./Dicelists"

function get_random_line {
  local FILE=$(shuf -n 1 ${DICELIST_DIR}/metalist.txt )
  # echo $FILE 1>&2
  local LINES=$(cat ${DICELIST_DIR}/${FILE})
  # echo $LINES 1>&2
  local LINE=$(echo "${LINES}" | shuf -n 1)
  echo $LINE
}


function get_word_from_line {
  echo $(shuf -n 1 -e $( echo $(get_random_line) | sed -e 's,\s+, ,g' | cut -d ' ' -f 2-))
}

NWORDS=$1

for i in $(seq 1 $NWORDS)
do
  get_word_from_line
done

