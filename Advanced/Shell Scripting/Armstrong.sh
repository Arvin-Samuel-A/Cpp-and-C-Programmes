#!/bin/bash

read -p "Enter the Number: " num;
temp=$num;

count=0;

while [[ $temp -gt 0 ]]; do
    ((count++));
    ((temp /= 10));
done;

sum=0;
temp=$num;

while [[ $temp -gt 0 ]]; do
    digit=$(($temp % 10));
    value=$(($digit ** $count));
    ((sum += value));
    ((temp /= 10));
done;

if [[ $sum -eq $num ]]; then
    echo "$num is a Armstrong Number";
else
    echo "$num is not a Armstrong Number";
fi