#!/bin/bash
try() {
    expected="$1"
    input="$2"

    ./jrcc "$input" > tmp.ll
    llc -filetype=obj -o tmp.o tmp.ll
    clang -o tmp tmp.o
    ./tmp
    actual="$?"

    if [ "$actual" = "$expected" ]; then
        echo "$input => $actual"
    else
        echo "$input => $expected expected, but got $actual"
        exit 1
    fi
}

try 0 0
try 42 42
try 16 " 34 + 56 - 74 "
try 47 '5+6*7'
try 15 '5*(9-6)'
try 4 '(3+5)/2'

echo OK