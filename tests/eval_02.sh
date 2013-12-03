#!/bin/bash


LD_LIBRARY_PATH=$LD_LIBRARY_PATH:../lib

rm -f ref.log acc*-*-*.log

for size in 20 23 27 30 35 39 44 50 57 64 72 81 90 101 113 126 140
do

  echo "size = $size"

  ./cpu_ref_02 $(($size*1000000)) >> ref.log

  echo "----------------------------"

  for gang in 1 2 4 8 16
  do
    for worker in 4 8 16 32 64
    do
      echo "size = $size"
      echo "gang = $gang"
      echo "worker = $worker"

      ./test_02_v1 $(($size*1000000)) $gang $worker >> acc1-$gang-$worker.log
      ./test_02_v2 $(($size*1000000)) $gang $worker >> acc2-$gang-$worker.log
      ./test_02_v3 $(($size*1000000)) $gang $worker >> acc3-$gang-$worker.log

      echo "----------------------------"
    done
  done
done

echo "" > plotter.gplot

echo -n "plot" >> plotter.gplot

for gang in 1 2 4 8 16 32
do
  for worker in 1 2 4 8 16 32 64 128
  do
    paste ref.log acc1-$gang-$worker.log | awk '{ print $1, $4, $5, $2/$6, $2/$7 }' > acc1-$gang-$worker.dat
    echo -n " \"acc1-$gang-$worker.log\" using 1:4 title \"T0=? G=$gang T1=1 W=$worker T2=1\"," >> plotter.gplot

    paste ref.log acc2-$gang-$worker.log | awk '{ print $1, $4, $5, $2/$6, $2/$7 }' > acc2-$gang-$worker.dat
    echo -n " \"acc2-$gang-$worker.log\" using 1:4 title \"T0=1 G=$gang T1=? W=$worker T2=1\"," >> plotter.gplot

    paste ref.log acc3-$gang-$worker.log | awk '{ print $1, $4, $5, $2/$6, $2/$7 }' > acc3-$gang-$worker.dat
    echo -n " \"acc3-$gang-$worker.log\" using 1:4 title \"T0=1 G=$gang T1=1 W=$worker T2=?\"," >> plotter.gplot
  done
done

gnuplot plotter.gplot

