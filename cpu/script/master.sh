#!/bin/bash
#SBATHC -N 1
#SBATCH -t 01:30:00
#SBATCH -J h5cpufreq
#SBATCH -o %j.out
#SBATCH -e %j.err
#SBATCH -C haswell  
#SBATCH -p regular

host=has # has,knl
code=h5 # h5 or dd_10g or dd_null
codename=h5write_${host} #h5write_knl
var=$(cat /sys/devices/system/cpu/cpu0/cpufreq/scaling_available_frequencies)
#var=(1400000 1300000 1200000)
hostname >> ${host}.${SLURM_JOBID}.bw 
echo $host >> ${host}.${SLURM_JOBID}.bw
echo $code >> ${host}.${SLURM_JOBID}.bw

for i in $var
do
 echo $i
 srun --cpu-freq=$i -n 1 --cpu_bind=cores ./${code}.sh $host $codename
done

#extract the IO data

if [ "$code" = "h5" ]; then
  grep 'Bandwidth' ${SLURM_JOBID}.err | awk '{ print $9 }' >> ${host}.${SLURM_JOBID}.bw 
else
  grep '10 G' ${SLURM_JOBID}.err | awk '{ print $8}' >> ${host}.${SLURM_JOBID}.bw  
fi
