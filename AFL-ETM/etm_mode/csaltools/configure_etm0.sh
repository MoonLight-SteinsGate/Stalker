#!/bin/sh

echo 1 > /sys/bus/coresight/devices/tmc_etr0/enable_sink;
#echo 0x200000 >/sys/bus/coresight/devices/tmc_etr0/buffer_size; #2MB
#echo 0x2000000 >/sys/bus/coresight/devices/tmc_etr0/buffer_size;  #32MB
#echo 0x10000000 >/sys/bus/coresight/devices/tmc_etr0/buffer_size #256MB
# echo 0x20000000 >/sys/bus/coresight/devices/tmc_etr0/buffer_size #512MB
# echo 0x40000000 >/sys/bus/coresight/devices/tmc_etr0/buffer_size #1G
echo 0x80000000  >/sys/bus/coresight/devices/tmc_etr0/buffer_size #2G

echo 0x4c000850 >/sys/bus/coresight/devices/etm0/mode;
echo 0 >/sys/bus/coresight/devices/etm0/addr_idx;
#echo 0x400000 0x40b000 0 >/sys/bus/coresight/devices/etm0/addr_range; #address
echo 0x400000 0x436000  0 >/sys/bus/coresight/devices/etm0/addr_range;
echo 0x101 >/sys/bus/coresight/devices/etm0/bb_ctrl;
    
echo 0 >/sys/bus/coresight/devices/etm0/ctxid_idx;
echo 0x3842 >/sys/bus/coresight/devices/etm0/ctxid_pid; #contextid
echo ctxid >/sys/bus/coresight/devices/etm0/addr_ctxtype;
    
echo 0 >/sys/bus/coresight/devices/etm0/cntr_idx;
echo 0 >/sys/bus/coresight/devices/etm0/cntr_ctrl;
echo 0 >/sys/bus/coresight/devices/etm0/cntrldvr;
echo 2 >/sys/bus/coresight/devices/etm0/res_idx;
echo 0x20001 > /sys/bus/coresight/devices/etm0/res_ctrl;
echo 0x2 > /sys/bus/coresight/devices/etm0/event_ts;