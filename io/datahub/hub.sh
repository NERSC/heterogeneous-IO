#!/bin/bash
#SBATCH -N 2
#SBATCH -t 00:10:00
#SBATCH -J hubio
#SBATCH -o %j.out
#SBATCH -e %j.err
#SBATCH -C haswell  
#SBATCH -p debug

#i=1100000  1100000, 1200000, 1300000, 1400000, 1401000, knl cpu frequencies
freq= 2301000 #1200000 # 1300000, 1400000, 1500000, 1600000, 1700000, 1800000, 1900000, 2000000, 2100000, 2200000, 2300000, 2301000, haswell cpu frequencies


nprocs=2
cbn=1         #Aggregators 32
cbs=16777216   #Collective Buffer size 16MB
iscollective=1 #Collective IO
dimx=2000     #Size of X dimension
dimy=10000    #Size of Y dimension
SCRATCH_OLD=/global/cscratch1/sd/jialin.old
lost=1        
hostpartion=has

filename=$SCRATCH_OLD/hdf-data/ost${lost}/${hostpartion}/test_${hostpartion}_${nprocs}_${freq}.h5
var=`scontrol show -d hostname | awk '{ $1 = substr($1, 4, 5) } 1'`
arr=($var)
hasmax=${arr[0]}
knlmax=${arr[1]}
echo $hasmax
echo $knlmax
srun --cpu-freq=$freq --tasks-per-node=1 -n $nprocs  ./test_get_procid -f $filename -b $cbs -n $cbn -k $iscollective -x $dimx -y $dimy -l $hasmax -m $knlmax 

h5dump -H $filename
lfs getstripe $filename
