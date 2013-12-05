#!/bin/bash

source $1/test-vars.rc

mkdir -p $1/datas
rm -f $1/datas/$2.dat

for size in $LIBACC_TEST_SIZE_LIST
do
  for gang in $LIBACC_TEST_GANG_LIST
  do
    for worker in $LIBACC_TEST_WORKER_LIST
    do
      for vector in $LIBACC_TEST_VECTOR_LIST
      do
        echo "size = $size"
        echo "gang = $gang"
        echo "worker = $worker"
        echo "vector = $vector"

        for i in `seq 1 $LIBACC_TEST_SAMPLING_SIZE`
        do
          echo -n "."
          ./$2 $(($size*1024*1024)) $gang $worker $vector >> $1/datas/$2.dat
        done

        echo;echo "----------------------------"
      done
    done
  done
done

