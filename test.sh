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

echo OK