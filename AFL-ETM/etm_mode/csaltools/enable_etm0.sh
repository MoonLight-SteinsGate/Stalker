#!/bin/sh
#/usr/bin/gcore -o /home/root/coredump/core.nginx 2697 2698

echo 1 >/sys/bus/coresight/devices/etm0/enable_source;