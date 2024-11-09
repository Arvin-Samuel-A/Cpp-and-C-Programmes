#!/bin/bash

arr=(12 77 34 154 770 10 20);

for num in ${arr[@]}; do
    if (($num % 11 == 0 && $num % 7 == 0)); then
        echo $num;
    fi
done