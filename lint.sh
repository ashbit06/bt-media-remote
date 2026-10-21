#!/bin/bash

FILE=$1

cpplint --extensions=ino "$FILE"
