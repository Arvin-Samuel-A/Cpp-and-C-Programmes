#!/bin/bash

prime() {

    if [[ $1 -le 1 ]]; then
        return 1;
    elif [[ $1 -eq 2 ]]; then
        return 0;
    elif (($1 % 2 == 0)); then
        return 1;
    else
        temp=3;
        val=$(echo "scale=0; sqrt($1);" | bc -l);
        found=0;

        while [[ $temp -le $val ]]; do
            rem=$(($1 % temp));
            if [[ $rem -eq 0 ]]; then
                found=1;
                break;
            fi
            ((temp += 2));
        done

        return $found;
    fi
}

echo -e "The List of Primes from 1 to 100 are:";

for num in {1..100}; do
    if prime $num; then
        echo $num;
    fi
done