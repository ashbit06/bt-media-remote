#!/bin/bash

FILE=$1

DIR=$(dirname "$FILE")
NAME=$(basename "$FILE")

TEMP="$DIR/temp_$NAME.cpp"
cp $FILE $TEMP
cpplint $TEMP

rm -f "$TEMP"