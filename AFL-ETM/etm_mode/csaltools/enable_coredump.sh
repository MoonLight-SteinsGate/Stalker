#!/bin/sh

echo "/home/root/coredump/core.%e.%p.%s.%t" > /proc/sys/kernel/core_pattern;
echo 2 > /proc/sys/fs/suid_dumpable