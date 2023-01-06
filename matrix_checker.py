from math import sqrt

with open('matrix.txt', 'r') as f:
    matrix = [list(map(float, l.split())) for l in f.read().strip().split('\n')]
    
dim = len(matrix)
print("matrix dim =", dim)

maxInd = int(round(sqrt(sqrt(dim))))
print("max index =", maxInd)
    
num_non_zero = 0
for row in matrix:
    for elem in row:
        if elem not in [0., -0.]:
            num_non_zero += 1

size = dim*dim
print("number of non-zero =", num_non_zero, "out of", size)
print("=", round(100 * num_non_zero/float(size),2), "%")
