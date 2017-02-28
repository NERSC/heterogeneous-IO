#!/bin/bash
#SBATHC -N 1
#SBATCH -t 00:30:00
#SBATCH -J cpufreq
#SBATCH -o %j.out
#SBATCH -e %j.err
#SBATCH -C haswell  
#SBATCH -p debug

host=has # has,knl
code=h5 # dd_10g or dd_null
var=$(cat /sys/devices/system/cpu/cpu0/cpufreq/scaling_available_frequencies)

hostname >> ${host}.${SLURM_JOBID}.bw 
echo $host >> ${host}.${SLURM_JOBID}.bw
echo $code >> ${host}.${SLURM_JOBID}.bw

for i in $var
do
 echo $i
 srun --cpu-freq=$i -n 1 ./${code}.sh 
done

#extract the IO data

if [ "$code" = "h5" ]; then
  grep 'Bandwidth' ${SLURM_JOBID}.err | awk '{ print $9 }' >> ${host}.${SLURM_JOBID}.bw 
else
  grep '10 G' ${SLURM_JOBID}.err | awk '{ print $8}' >> ${host}.${SLURM_JOBID}.bw  
fi
