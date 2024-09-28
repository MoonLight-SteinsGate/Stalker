#!/bin/sh

time2=$(date "+%Y%m%d%H%M%S");
echo $time2
touch ~/trace/etmtrace_$time2;
dd if=/dev/tmc_etr0 of=/home/root/trace/etmtrace_$time2;