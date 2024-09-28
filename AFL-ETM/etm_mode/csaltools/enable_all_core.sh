#!/bin/sh


echo 1 > /sys/devices/system/cpu/cpu0/online; # a72_0
echo 1 > /sys/devices/system/cpu/cpu1/online; # a72_1
echo 1 > /sys/devices/system/cpu/cpu2/online; # a53_0
echo 1 > /sys/devices/system/cpu/cpu3/online; # a53_1
echo 1 > /sys/devices/system/cpu/cpu4/online; # a53_2
echo 1 > /sys/devices/system/cpu/cpu5/online; # a53_3