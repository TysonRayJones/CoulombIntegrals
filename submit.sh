#!/bin/bash
 
#SBATCH --account=oums-quantopo
#SBATCH --partition=short
#SBATCH --constraint=cpu_gen:Cascade_Lake
#SBATCH --constraint=cpu_sku:Platinum_8628
#SBATCH --clusters=arc 
 
#SBATCH --nodes=1
#SBATCH --time=50:00:00
#SBATCH --exclusive
#SBATCH --mem=40960

module load GCCcore/10.2.0

#chmod +x compiler.sh
# ./compiler.sh
 
#export OMP_NUM_THREADS=96
#./calcer 15 matrix.txt


for nthreads in 1 2 4 8 16 32 48 64 96; do
    echo '1 thread:' >> timer.txt
    export OMP_NUM_THREADS=$nthreads
    { time ./calcer 8 trash.txt ; } 2>> timer.txt
done