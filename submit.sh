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

chmod +x compiler.sh
./compiler.sh -DUSE_CACHE
 
export OMP_NUM_THREADS=96
./calcer 15 matrix.txt