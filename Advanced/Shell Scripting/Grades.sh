#!/bin/bash

grade="F";

if [[ $1 -lt 0 ]] || [[ $1 -gt 100 ]]; then
    echo "You have entered the wrong marks";
    exit 1;
elif [[ $1 -ge 90 ]]; then
    grade="S";
elif [[ $1 -ge 80 ]]; then
    grade="A";
elif [[ $1 -ge 70 ]]; then
    grade="B";
elif [[ $1 -ge 60 ]]; then
    grade="C";
elif [[ $1 -ge 50 ]]; then
    grade="D";
elif [[ $1 -ge 40 ]]; then
    grade="E";
fi

echo "Your grade is $grade";