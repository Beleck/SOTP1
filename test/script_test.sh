#!/usr/bin/env bash

mkdir -p dir_test
for i in {0..10}; do 
    touch dir_test/$i
    dd if=/dev/urandom bs=1024 of=dir_test/$i count=1 2> /dev/null
done
cd ..
./application test/dir_test/*
#md5sum test/dir_test/*
#rm -r test/dir_test
