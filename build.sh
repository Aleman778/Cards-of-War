#!/bin/bash

code="$PWD"
opts=-g
cd build > /dev/null
g++ $opts $code/main.o -o build
cd $code > /dev/null
