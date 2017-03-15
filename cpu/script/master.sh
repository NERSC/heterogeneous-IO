#!/bin/bash
#SBATHC -N 1
#SBATCH -t 00:30:00
#SBATCH -J h5cpufreq
#SBATCH -o %j.out
#SBATCH -e %j.err
#SBATCH -C knl  
#SBATCH -p regular
##SBATCH --vtune
#module load vtune

host=knl # has,knl
code=dd_10g # h5 or dd_10g or dd_null
var=$(cat /sys/devices/system/cpu/cpu0/cpufreq/scaling_available_frequencies)
var=(1400000 1300000 1200000)
hostname >> ${host}.${SLURM_JOBID}.bw 
echo $host >> ${host}.${SLURM_JOBID}.bw
echo $code >> ${host}.${SLURM_JOBID}.bw
res_dir=$SCRATCH/vtune_log
mkdir -p $res_dir

for i in 1401000
do
 echo $i
 #i=1400000
 #srun --cpu-freq=$i -n 1 --cpu_bind=cores amplxe-cl -collect general-exploration -r $res_dir -trace-mpi  ./${code}.sh $host
 srun --cpu-freq=$i -n 1 --cpu_bind=cores ./${code}.sh $host
done

#extract the IO data

if [ "$code" = "h5" ]; then
  grep 'Bandwidth' ${SLURM_JOBID}.err | awk '{ print $9 }' >> ${host}.${SLURM_JOBID}.bw 
else
  grep '10 G' ${SLURM_JOBID}.err | awk '{ print $8}' >> ${host}.${SLURM_JOBID}.bw  
fi
