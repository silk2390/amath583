#!/usr/bin/env bash

for i in {1..1048576}
do
    echo "===================================================="
    echo "i: $i"
    echo "===================================================="
    ./norm.exe $i
done
