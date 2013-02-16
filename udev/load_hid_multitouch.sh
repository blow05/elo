#!/bin/bash

input=$@

/sbin/modprobe hid-multitouch
echo 3 $input 0 > /sys/module/hid_multitouch/drivers/hid\:hid-multitouch/new_id
