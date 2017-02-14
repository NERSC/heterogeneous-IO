#!/bin/bash
#SBATHC -N 1
#SBATCH -t 1:30:00
#SBATCH -J cpufreq
#SBATCH -o %j.out
#SBATCH -e %j.err
#SBATCH -C haswell
#SBATCH -p regular
host="haswell"
var=$(cat /sys/devices/system/cpu/cpu0/cpufreq/scaling_available_frequencies)

#for i in 1200000 1300000
hostname >> ${host}.${SLURM_JOBID}.bw 
echo $host >> ${host}.${SLURM_JOBID}.bw
for i in $var
do
 #echo $i >>${host}.${SLURM_JOBID}.bw
 echo $i
 srun --cpu-freq=$i -n 1 ./sio.sh #> ${SLURM_JOBID}.bw # sio.sh contains the io job, bandwidth test
done
grep '10 G' ${SLURM_JOBID}.err | awk '{ print $8}' >> ${host}.${SLURM_JOBID}.bw  

