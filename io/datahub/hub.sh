#!/bin/bash
#SBATCH -N 2
#SBATCH -t 00:10:00
#SBATCH -J hubio
#SBATCH -o %j.out
#SBATCH -e %j.err
#SBATCH -C haswell  
#SBATCH -p debug

#i=1100000  1100000, 1200000, 1300000, 1400000, 1401000, knl cpu frequencies
freq=2300000 #1200000 # 1300000, 1400000, 1500000, 1600000, 1700000, 1800000, 1900000, 2000000, 2100000, 2200000, 2300000, 2301000, haswell cpu frequencies

nodes=2
rank_per_node=32
nprocs=$(($nodes * $rank_per_node))
cbn=2          #Aggregators 32
cbs=16777216   #Collective Buffer size 16MB
iscollective=0 #Collective IO
unit=2000
dimx=$(($unit * $nprocs))     #Size of X dimension
dimy=102400     #Size of Y dimension
size_per_proc=$(($dimx * $dimy)) # 1.52GB
SCRATCH_OLD=/global/cscratch1/sd/jialin
lost=72        
hostpartion=has
directio=1     #1 for direct io, 0 for io offloading
filename=/global/cscratch1/sd/jialin/hdf-data/ost${lost}/${hostpartion}/test_${hostpartion}_${nprocs}_${freq}_${directio}.h5
var=`scontrol show -d hostname | awk '{ $1 = substr($1, 4, 5) } 1'`
arr=($var)

hasmax=${arr[$((($nodes-1)/2))]}
knlmax=${arr[$(($nodes-1))]}
echo $hasmax
echo $knlmax
rm $SCRATCH_OLD/hdf-data/ost${lost}/${hostpartion}/*
#Test original IO
srun --cpu-freq=$freq --tasks-per-node=32 -n $nprocs  ./test_get_procid -f $filename -b $cbs -n $cbn -k $iscollective -x $dimx -y $dimy -l $hasmax -m $knlmax -d $directio

#h5dump -H $filename
#lfs getstripe $filename

#Test IO Offloading
#directio=0
#filename=$SCRATCH_OLD/hdf-data/ost${lost}/${hostpartion}/test_${hostpartion}_${nprocs}_${freq}_${directio}.h5
#srun --cpu-freq=$freq --tasks-per-node=32 -n $nprocs  ./test_get_procid -f $filename -b $cbs -n $cbn -k $iscollective -x $dimx -y $dimy -l $hasmax -m $knlmax -d $directio

#h5dump -H $filename
#lfs getstripe $filename
