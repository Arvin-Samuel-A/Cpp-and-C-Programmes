#!/bin/bash

#cat s.txt;

while IFS= read line; do
    echo $line;
done < s.txt