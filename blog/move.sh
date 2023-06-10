#!/bin/sh

if [ ! -d bin/module ]
then
    mkdir bin/module
else
    unlink bin/blog
    unlink bin/module/libblog.so
fi

cp game_server/bin/sylar bin/blog
cp lib/libblog.so bin/module/