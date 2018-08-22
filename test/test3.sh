#!/usr/bin/env bash

echo "BEGINNING OF TEST 3 WITH RES FILENAME"
./application -f res test/dir_test/* > /dev/null
RESULTS=($(cat res | sed 's/://g' | tr '\n' ' '))
for i in $(seq 0 $NUM_FILES); do
    if [ ! "${RESULTS[2*$i+1]}" = "$(md5sum ${RESULTS[2*$i]} | cut -d' ' -f 1)" ]; then
       exit 1
    fi
done
rm res
exit 0
