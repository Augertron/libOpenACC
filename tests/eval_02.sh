#!/bin/bash


LD_LIBRARY_PATH=$LD_LIBRARY_PATH:../lib

size=$((1024*1024))

for i in {1..10}
do
#  echo "========================="
#  echo "size   = $size"
#  echo "-------------------------"
#  echo -n "reference : "
  echo -n "ref  "
  ./cpu_ref_02 $size

  gang=2
  for j in {1..5}
  do
#    echo "-------------------------"
#    echo "gang   = $gang"

    worker=2
    for k in {1..6}
    do
#      echo "-------------------------"
#      echo "worker = $worker"
#      echo "-------------------------"

#      echo -n "version 1 : "
      echo -n "acc1 "
      ./test_02_v1 $size $gang $worker
#      echo -n "version 2 : "
      echo -n "acc2 "
      ./test_02_v2 $size $gang $worker
#      echo -n "version 3 : "
      echo -n "acc3 "
      ./test_02_v3 $size $gang $worker

      worker=$((2*$worker))
    done

    gang=$((2*$gang))
  done

  size=$((2*$size))
done

