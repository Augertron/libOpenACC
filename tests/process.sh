#!/bin/bash

source $1/test-vars.rc

echo "size,time" > $1/datas/$3.csv

cat $1/datas/$3.dat | awk -v reps=$LIBACC_TEST_SAMPLING_SIZE '\
                             BEGIN {\
                               i=0;\
                               s=0;\
                             }\
                             {\
                               s=s+$2;\
                               i++;\
                               if (i==reps) {\
                                 print $1 "," s;\
                                 s=0
                                 i=0;\
                               }\
                             }' >> $1/datas/$3.csv

for file in `ls $1/datas/$2-*.dat | sed s/\.dat$//`
do
  cat $file.dat  | awk -v reps=$LIBACC_TEST_SAMPLING_SIZE '\
                             BEGIN {\
                               i=0;\
                               s1=0;\
                               s2=0;\
                             }\
                             {\
                               s1=s1+$5;\
                               s2=s2+$6;\
                               i++;\
                               if (i==reps) {\
                                 print $1 "," $2 "," $3 "," $4 "," s1 "," s2;\
                                 s1=0
                                 s2=0
                                 i=0;\
                               }\
                             }' > $file.tmp

  
  echo "size,gang,worker,vector,comp-speedup,comp-comm-speedup,comp-time,comm-time" > $file.csv
  for size in $LIBACC_TEST_SIZE_LIST
  do
    cat $file.tmp | grep "^$size" | awk -v tref=`cat $1/datas/$3.csv | grep "^$size" | awk 'BEGIN{FS=","}{print $2}'` '\
                                                    BEGIN{FS=","}\
                                                    {\
                                                      print $1 "," $2 "," $3 "," $4 "," tref/$5 "," tref/$6 "," $5 "," $6-$5 ;\
                                                    }' >> $file.csv
  done

  for gang in $LIBACC_TEST_GANG_LIST
  do
    for worker in $LIBACC_TEST_WORKER_LIST
    do
      for vector in $LIBACC_TEST_VECTOR_LIST
      do
        echo "size,gang,worker,vector,comp-speedup,comp-comm-speedup,comp-time,comm-time" > $file-by_size-gang_$gang-worker_$worker-vector_$vector.csv
        cat $file.csv | grep "$gang,$worker,$vector" >> $file-by_size-gang_$gang-worker_$worker-vector_$vector.csv
      done
    done
  done
done

# TODO By   size: one data point per gang/worker/vector tested + mean + stddev

# TODO By   gang/worker: 


#for size in $LIBACC_TEST_SIZE_LIST
#do
#  for gang in $LIBACC_TEST_GANG_LIST
#  do
#    for worker in $LIBACC_TEST_WORKER_LIST
#    do
#      for vector in $LIBACC_TEST_VECTOR_LIST
#      do
#      done
#    done
#  done
#done

