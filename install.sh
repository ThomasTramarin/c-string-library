#!/bin/bash

mkdir sl_string

echo "Downloading library files..."

curl -s -o sl_string/sl_string.h https://raw.githubusercontent.com/ThomasTramarin/c-string-library/main/include/sl_string.h
curl -s -o sl_string/sl_string.c https://raw.githubusercontent.com/ThomasTramarin/c-string-library/main/src/sl_string.c

echo "Library installed in ./sl_string"
echo "You can now include sl_string.h and compile sl_string.c in your project"