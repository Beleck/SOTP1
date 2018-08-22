#!/usr/bin/env bash

echo "BEGINNING OF TEST 2 WITH NO VIEWER"
./application test/dir_test/* > /dev/null
RESULTS=($(cat results.res | sed 's/://g' | tr '\n' ' '))
TEST=0
for i in $(seq 0 $NUM_FILES); do
    if [ ! "${RESULTS[2*0+1]}" = "$(md5sum ${RESULTS[2*0]} | cut -d' ' -f 1)" ]; then
        TEST=1
    fi
done
if [ $TEST = 0 ]; then
    echo "***************** TEST 2 PASSED ***************"
else 
    echo "///////////////// TEST 2 FAILED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\ "
fi
