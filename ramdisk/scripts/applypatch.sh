#!/bin/sh

mkdir -p $1/system

[ -e $1/init.rc.append ] && cat $1/init.rc.append >> $1/init.rc
[ -e $1/init.smdkv210.rc.append ] && cat $1/init.smdkv210.rc.append >> $1/init.smdkv210.rc
[ -e $1/build.prop.append ] && cat $1/build.prop.append >> $1/system/build.prop

rm -f $1/init.rc.append
rm -f $1/init.smdkv210.rc.append
rm -f $1/build.prop.append

