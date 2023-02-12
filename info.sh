#!/bin/bash

# clear_screen=\E[H\E[J,
# cursor_address=\E[%i%p1%d;%p2%dH,
# set_a_background=\E[4%p1%dm,
# set_a_foreground=\E[3%p1%dm,

echo -e "\e[H\e[J"
echo -e "\e[31m"
echo -e "\e[40m"
echo -e -n "\e[5;10H"
echo -e "Kokorin Sergey"
echo -e "\e[32m"
echo -e "\e[47m"
echo -e -n "\e[6;8H"
echo -e "IV-121"
echo -e "\e[39m"
echo -e "\e[49m"
echo -e "\e[10;1H"
