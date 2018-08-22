#!/usr/bin/env bash
#Max 170 with BASE_SIZE 10000 in include/app_shm.c
export NUM_FILES=170

#Creation of files to test
mkdir -p dir_test
for i in $(seq 0 $NUM_FILES); do 
    touch dir_test/$i
    dd if=/dev/urandom bs=1024 of=dir_test/$i count=10 2> /dev/null
done

cd ..
NB_SCRIPT=$(ls test/test* | wc -w)
for i in $(seq 1 $NB_SCRIPT); do
    test/test"$i".sh
    if [ $? = 0 ]; then
        echo "***************** TEST $i PASSED ***************"
    else 
        echo "///////////////// TEST $i FAILED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\ "
    fi
done



rm -r test/dir_test
