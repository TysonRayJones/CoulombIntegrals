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


n=8
fn=timer.txt
tr=trash.txt
echo 'hello\n' > $fn


echo '\n\n1 thread:\n' >> $fn
export OMP_NUM_THREADS=1
time ./calcer $n $tr >> $fn

echo '\n\n2 threads:\n' >> $fn
export OMP_NUM_THREADS=2
time ./calcer $n $tr >> $fn

echo '\n\n4 threads:\n' >> $fn
export OMP_NUM_THREADS=4
time ./calcer $n $tr >> $fn

echo '\n\n8 threads:\n' >> $fn
export OMP_NUM_THREADS=8
time ./calcer $n $tr >> $fn

echo '\n\n16 threads:\n' >> $fn
export OMP_NUM_THREADS=16
time ./calcer $n $tr >> $fn

echo '\n\n32 threads:\n' >> $fn
export OMP_NUM_THREADS=32
time ./calcer $n $tr >> $fn

echo '\n\n48 threads:\n' >> $fn
export OMP_NUM_THREADS=48
time ./calcer $n $tr >> $fn

echo '\n\n64 threads:\n' >> $fn
export OMP_NUM_THREADS=64
time ./calcer $n $tr >> $fn

echo '\n\n96 threads:\n' >> $fn
export OMP_NUM_THREADS=96
time ./calcer $n $tr >> $fn