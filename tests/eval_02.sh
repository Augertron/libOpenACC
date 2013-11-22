#!/bin/bash


LD_LIBRARY_PATH=$LD_LIBRARY_PATH:../lib

size=$((1024*1024))

for i in {1..8}
do
  gang=2
  for j in {$gang..5}
  do
    worker=4
    for k in {$worker..8}
    do
      echo "-------------------------"
      echo "size   = $size"
      echo "gang   = $gang"
      echo "worker = $worker"
      echo -n "reference : "
      ./cpu_ref_02 $size
      echo -n "version 1 : "
      ./test_02_v1 $size $gang $worker
      echo -n "version 2 : "
      ./test_02_v2 $size $gang $worker
      echo -n "version 3 : "
      ./test_02_v3 $size $gang $worker

      worker=$((2*$worker))
    done

    gang=$((2*$gang))
  done

  size=$((2*$size))
done

