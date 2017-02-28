#!/bin/bash
nodes=1       #Nodes 32
nprocs=2    #Processes 1024
cbn=1         #Aggregators 32
cbs=16777216   #Collective Buffer size 16MB
iscollective=0 #Collective IO
dimx=320     #Size of X dimension
dimy=300    #Size of Y dimension
lost=1        #OST 72
hostpartion=has
SCRATCH_OLD=/global/cscratch1/sd/jialin.old
for j in 1 2 3 
do
  echo "cpu0:$(cat /sys/devices/system/cpu/cpu0/cpufreq/scaling_cur_freq)"
  freq=$(cat /sys/devices/system/cpu/cpu0/cpufreq/scaling_cur_freq)
  filename=$SCRATCH_OLD/hdf-data/ost${lost}/${hostpartion}/test_${hostpartion}_${nprocs}_${freq}_${j}.h5
  #cmd="srun -n $nprocs hostname " 
 
  cmd="srun -n $nprocs ./h5write -f $filename -b $cbs -n $cbn -k $iscollective -x $dimx -y $dimy"
  echo $hostpartion
  echo $cmd
  $cmd
  ls -alh $SCRATCH_OLD/hdf-data/ost${lost}/${hostpartion}/test_${hostpartion}_${nprocs}_${freq}_${j}.h5
  rm $SCRATCH_OLD/hdf-data/ost${lost}/${hostpartion}/test_${hostpartion}_${nprocs}_${freq}_${j}.h5
done
