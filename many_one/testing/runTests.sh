#!/bin/bash
echo "Starting Testing:"
make clean
make try 
make answer
make sync
./try
array1=(10 30 50 80 100)
for n in ${array1[@]}; 
do
    ./answer $n
done
./sync
