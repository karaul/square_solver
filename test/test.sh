#!/bin/bash

PROG=${1:-"../build/square_solver"}
TESTFILE=${2:-"test.txt"}

while read line; do 
    ARGS=$( echo $line ) # | sed 's/^.*exe//1;'  )
    echo  input:
    echo $PROG $ARGS
    echo output:
    $PROG  $ARGS
done <  ${TESTFILE}

