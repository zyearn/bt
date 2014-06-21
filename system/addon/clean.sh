#!/bin/bash


echo "0" > ./filenum
if [ -f ./filelib ];
then
    rm ./filelib
else
    echo "./filelib not exist"
fi
