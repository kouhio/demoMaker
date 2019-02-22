#!/bin/sh

cd ../GraphApi
make clean
make lib
cd ../demoMaker

make clean
cp ../GraphApi/GraphAPI.lib .
cp ../GraphApi/headers/* ./headers/
make
./demoPlayer

