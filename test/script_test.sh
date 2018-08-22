#!/usr/bin/env bash
#Max 237 with BASE_SIZE 10000 in include/app_shm.c
NUM_FILES=237
echo "BEGINNING OF TEST 1 WITH $NUM_FILES FILES"
mkdir -p dir_test
for i in $(seq 0 $NUM_FILES); do 
    touch dir_test/$i
    dd if=/dev/urandom bs=1024 of=dir_test/$i count=1 2> /dev/null
done
cd ..
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
rm -r test/dir_test
