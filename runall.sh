#!/bin/bash
echo "Starting Testing For one-one:" 

cc one_one/testing/libraryFunctionsTest.c one_one/mythread.c one_one/lock.c -o try1
cc one_one/testing/matrixMultiTest.c one_one/mythread.c one_one/lock.c -o answer1
cc one_one/testing/sync_test.c one_one/mythread.c one_one/lock.c -o sync1

./try1
array1=(30 50 100 1000)
for n in ${array1[@]};
do
    ./answer1 $n
done
./sync1

cc many_one/testing/libraryFunctionsTest.c many_one/mythread.c many_one/lock.c -o try1
cc many_one/testing/matrixMultiTest.c many_one/mythread.c many_one/lock.c -o answer1
cc many_one/testing/sync_test.c many_one/mythread.c many_one/lock.c -o sync1

./try1
array1=(5 10 30 50 100)
for n in ${array1[@]};
do
    ./answer1 $n
done
./sync1





rm try1
rm answer1
rm sync1
