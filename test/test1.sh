#!/usr/bin/env bash

echo "BEGINNING OF TEST 1 WITH $NUM_FILES FILES"
RESULTS=($(./application test/dir_test/* | ./viewer | sed 's/://g' | tr '\n' ' '))
TEST=0
for i in $(seq 0 $NUM_FILES); do
    if [ ! "${RESULTS[2*0+1]}" = "$(md5sum ${RESULTS[2*0]} | cut -d' ' -f 1)" ]; then
        TEST=1
    fi
done

if [ $TEST = 0 ]; then
    echo "***************** TEST 1 PASSED ***************"
else 
    echo "///////////////// TEST 1 FAILED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\ "
fi
