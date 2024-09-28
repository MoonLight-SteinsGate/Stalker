#!/bin/sh

etm_configure(){
 
  echo 1 > /sys/bus/coresight/devices/tmc_etr0/enable_sink;
  #echo 0x200000 >/sys/bus/coresight/devices/tmc_etr0/buffer_size; #2MB
  #echo 0x2000000 >/sys/bus/coresight/devices/tmc_etr0/buffer_size;  #32MB
  echo 0x10000000 >/sys/bus/coresight/devices/tmc_etr0/buffer_size #256MB
  # echo 0x20000000 >/sys/bus/coresight/devices/tmc_etr0/buffer_size #512MB
  # echo 0x40000000 >/sys/bus/coresight/devices/tmc_etr0/buffer_size #1G

for i in 0 1 2 3 4 5 ; do
    echo 0x4c000850 >/sys/bus/coresight/devices/etm$i/mode;
    echo 0 >/sys/bus/coresight/devices/etm$i/addr_idx;
    # echo 0x400000 0x41a000  0 >/sys/bus/coresight/devices/etm$i/addr_range; #address
    echo 0x0 0xfffffffff  0 >/sys/bus/coresight/devices/etm$i/addr_range 
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
 
}


etm_enable(){
  # enable etm
  for i in 0 1 2 3 4 5 ; do
    echo 1 >/sys/bus/coresight/devices/etm$i/enable_source;
  done
}

etm_shutdown(){
  for i in 0 1 2 3 4 5 ; do
    echo 0 >/sys/bus/coresight/devices/etm$i/enable_source;
  done
}


etm_save()
{
  time=$(date "+%Y%m%d%H%M%S");
  echo $time
  touch ~/trace/etmtrace_$time;
  dd if=/dev/tmc_etr0 of=/home/root/trace/etmtrace_$time;
}


etm_save_test(){
  etm_shutdown;
  etm_save;

  etm_configure;
  etm_enable;
  for i in {1..50}; do
    sleep 100;
    etm_shutdown;
    output=`etm_save`;
    etm_enable;
  done

  etm_shutdown;
  etm_save;

}

etm_save_test


