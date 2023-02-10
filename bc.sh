#!/bin/bash

str="++\054\054--..00__``aaffgghhiijjkkllmmnnooppqqrrssttuuvvwwxxyyzz{{||}c~~";

for (( i=0; $i<${#str}; i=$(($i+1)) ))
do
echo -n -e "\e(0${str:$i:1}\e(B";
done
echo ""
