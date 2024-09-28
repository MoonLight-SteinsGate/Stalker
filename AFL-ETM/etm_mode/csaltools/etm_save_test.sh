#!/bin/sh

for i in {1..50}; do
   sleep 2;
   /home/root/csaltools/shutdown_all_etm.sh
   output=`/home/root/csaltools/save_etr.sh`
   /home/root/csaltools/enable_all_etm.sh
done

/home/root/csaltools/shutdown_all_etm.sh
/home/root/csaltools/save_etr.sh