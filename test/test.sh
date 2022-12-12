#!/bin/bash

while read line; do 
    ARGS=$( echo $line | sed 's/^.*exe//1;'  )
    echo 
    echo $ARGS
    ../build/square_solver $ARGS
done < test.txt

