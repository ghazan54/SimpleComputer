#!/bin/bash

echo -e "\e[f\e[J"
echo -e -n "\e[10;5H"

size=8

for (( i=0; $i<$size+1; i=$(($i+1)) ))
do
echo -n -e "\e(0a\e(B";
done

j=11
echo -e -n "\e[$j;5f"
for (( i=0; $i<$size; i=$(($i+1)) ))
do
echo -n -e "\e(0a\e(B";
j=$(($j+1))
echo -e -n "\e[$j;5f"
done

j=11
line=$(($size+5))
echo -e -n "\e[$j;"$line"f"
for (( i=0; $i<$size; i=$(($i+1)) ))
do
echo -n -e "\e(0a\e(B";
j=$(($j+1))
echo -e -n "\e[$j;"$line"f"
done

line=$(($size*2+2))
echo -e -n "\e[$line;5f"
for (( i=0; $i<$size; i=$(($i+1)) ))
do
echo -n -e "\e(0a\e(B";
done

#* 10

echo -e "\e[31m"
echo -e -n "\e[11;7f"
for (( i=0; $i<$size/2+1; i=$(($i+1)) ))
do
echo -n -e "\e(0a\e(B";
done

j=12
echo -e -n "\e[$j;6f"
for (( i=0; $i<$size-3; i=$(($i+1)) ))
do
echo -n -e "\e(0a\e(B";
j=$(($j+1))
echo -e -n "\e[$j;6f"
done

j=12
line=$(($size+4))
echo -e -n "\e[$j;"$line"f"
for (( i=0; $i<$size-3; i=$(($i+1)) ))
do
echo -n -e "\e(0a\e(B";
j=$(($j+1))
echo -e -n "\e[$j;"$line"f"
done

line=$(($size*2+2-1))
echo -e -n "\e[$line;7f"
for (( i=0; $i<$size/2+1; i=$(($i+1)) ))
do
echo -n -e "\e(0a\e(B";
done

echo -e -n "\e[39m"
echo -e -n "\e[49m"
echo -e "\n\n"
echo -e -n "\e[0;0f"
