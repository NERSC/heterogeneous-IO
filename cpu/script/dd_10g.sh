#!/bin/bash
sleep 10
hostpart=$1 
SCRATCH_OLD=/global/cscratch1/sd/jialin
mkdir -p $SCRATCH_OLD/hk/cpu/${hostpart}
for j in 1 2 3 
do
  echo $i":"$j
  echo "cpu0:$(cat /sys/devices/system/cpu/cpu0/cpufreq/scaling_cur_freq)"
  echo "cpu12:$(cat /sys/devices/system/cpu/cpu12/cpufreq/scaling_cur_freq)"
  echo "cpu31:$(cat /sys/devices/system/cpu/cpu31/cpufreq/scaling_cur_freq)"
  freq=$(cat /sys/devices/system/cpu/cpu31/cpufreq/scaling_cur_freq)
  perf stat -B dd if=/dev/zero of=$SCRATCH_OLD/hk/cpu/${hostpart}/${freq}_${j}.txt bs=1M count=10000 
  ls -alh $SCRATCH_OLD/hk/cpu/${hostpart}/${freq}_${j}.txt
  rm $SCRATCH_OLD/hk/cpu/${hostpart}/${freq}_${j}.txt
done


