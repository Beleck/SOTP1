#!/usr/bin/env bash

echo "BEGINNING OF TEST 3 WITH RES FILENAME"
./application -f res test/dir_test/* > /dev/null
RESULTS=($(cat res | sed 's/://g' | tr '\n' ' '))
TEST=0
for i in $(seq 0 $NUM_FILES); do
    if [ ! "${RESULTS[2*0+1]}" = "$(md5sum ${RESULTS[2*0]} | cut -d' ' -f 1)" ]; then
       TEST=1
    fi
done
if [ $TEST = 0 ]; then
    echo "***************** TEST 3 PASSED ***************"
else 
    echo "///////////////// TEST 3 FAILED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\ "
fi
