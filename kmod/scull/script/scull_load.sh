#!/bin/bash
module="scull"
device="scull"
mode="664"

# invoke insmod with all arguments, if fails exit the script
/sbin/insmod ./${module}.ko $* || exit 1

# remove stale nodes
rm -f /dev/${device}[0-3]

major=$(awk "\\$2= =\"$module\" {print \\$1}" /proc/devices)

mknod /dev/${device}0 c $major 0
mknod /dev/${device}1 c $major 1
mknod /dev/${device}2 c $major 2
mknod /dev/${device}3 c $major 3

chmod $mode /dev/${device}[0-3]