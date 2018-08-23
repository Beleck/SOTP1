#!/usr/bin/env bash

echo "BEGINNING OF TEST 5 WITH 10 WORKER AND 5 FILES"
RESULTS=($(./application -w 10 test/dir_test/* | ./viewer | sed 's/://g' | tr '\n' ' '))
for i in $(seq 0 5); do
    if [ ! "${RESULTS[2*$i+1]}" = "$(md5sum ${RESULTS[2*$i]} | cut -d' ' -f 1)" ]; then
        exit 1
    fi
done
rm results.res
exit 0
