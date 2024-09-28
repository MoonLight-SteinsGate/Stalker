#!/bin/sh
for i in 0 1 2 3 4 5; do
   echo 1 >/sys/bus/coresight/devices/etm$i/enable_source;
done
