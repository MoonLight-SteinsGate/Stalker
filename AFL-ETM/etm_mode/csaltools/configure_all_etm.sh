#!/bin/sh

echo 1 > /sys/bus/coresight/devices/tmc_etr0/enable_sink;
#echo 0x200000 >/sys/bus/coresight/devices/tmc_etr0/buffer_size; #2MB
#echo 0x2000000 >/sys/bus/coresight/devices/tmc_etr0/buffer_size;  #32MB
echo 0x10000000 >/sys/bus/coresight/devices/tmc_etr0/buffer_size #256MB
# echo 0x20000000 >/sys/bus/coresight/devices/tmc_etr0/buffer_size #512MB
#echo 0x80000000 >/sys/bus/coresight/devices/tmc_etr0/buffer_size #2G

for i in 0 1 2 3 4 5 ; do
    echo 0x4c000850 >/sys/bus/coresight/devices/etm$i/mode;
    echo 0 >/sys/bus/coresight/devices/etm$i/addr_idx;
  echo 0x400000 0x54f000 0 >/sys/bus/coresight/devices/etm$i/addr_range; #address
  # echo 0x0 0xfffffffff  0 >/sys/bus/coresight/devices/etm$i/addr_range 
   echo 0x101 >/sys/bus/coresight/devices/etm$i/bb_ctrl;
    
    #echo 0 >/sys/bus/coresight/devices/etm$i/ctxid_idx;
    #echo 0x146>/sys/bus/coresight/devices/etm$i/ctxid_pid; #contextid
    #echo ctxid >/sys/bus/coresight/devices/etm$i/addr_ctxtype;
    
    echo 0 >/sys/bus/coresight/devices/etm$i/cntr_idx;
    echo 0 >/sys/bus/coresight/devices/etm$i/cntr_ctrl;
    echo 0 >/sys/bus/coresight/devices/etm$i/cntrldvr;
    echo 2 >/sys/bus/coresight/devices/etm$i/res_idx;
    echo 0x20001 > /sys/bus/coresight/devices/etm$i/res_ctrl;
    echo 0x2 > /sys/bus/coresight/devices/etm$i/event_ts;


done
