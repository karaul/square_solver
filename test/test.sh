#!/bin/bash

MAXCOUNT=50
count=1

test="$1"

while [ "$count" -le $MAXCOUNT ]; do
 test=$test" "$RANDOM
 let "count += 1"
done

echo $test