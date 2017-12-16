#!/bin/sh

echo 'sudo umount /mnt/ramdisk'
sudo umount /mnt/ramdisk

echo 'sudo rmmod nova'
sudo rmmod nova

echo 'sudo insmod nova.ko measure_timing=0'
sudo insmod nova.ko measure_timing=0

echo 'sleep 1'
sleep 1

echo 'sudo mount -t NOVA -o init /dev/pmem0 /mnt/ramdisk'
sudo mount -t NOVA -o init /dev/pmem0 /mnt/ramdisk

echo 'sudo chown jchoi:jchoi /mnt/ramdisk -R'
sudo chown jchoi:jchoi /mnt/ramdisk -R

echo 'df -lh'
df -lh

echo 'cp ~/Workplace/test/testfile/testfile /mnt/ramdisk'
cp ~/Workplace/test/testfile/testfile /mnt/ramdisk
