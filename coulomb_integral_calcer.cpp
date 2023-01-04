#include <math.h>
#include <stdio.h>
#include <cmath>
#include <initializer_list>
#include <array>
#include <fstream>


using std::array;
using std::ofstream;


double inline factorial(int n) {
    // double to avoid int overflow
    double f = 1;
    while (n > 1)
        f *= n--;
    return f;
}

double inline doubleFactorial(int n) {
    // double to avoid int overflow
    double f = 1;
    while (n > 1) {
        f *= n;
        n -= 2;
    }
    return f;
}

double inline combinations(int n, int r) {
    // double to avoid int overflow
    double f = 1;
    int j = n;
    while (j >= (n-r+1))
        f *= (j--);
    return f/factorial(r);
}

double inline productOfCombinations(int a, int b, int p) {
    return factorial(p) * combinations(a, p) * combinations(b, p);
}

int inline min(int a, int b) {
    return (a <= b)? a : b;
}

int inline isOdd(int n) {
    return n%2;
}

int inline powerOfNegativeOne(int p) {
    return 1 + -2*isOdd(p);
}

double getIntegral(array<int,8> inds) {
    auto [n_a, m_a, n_b, m_b, n_g, m_g, n_d, m_d] = inds;
    
    int n_sum = n_a + n_d + n_b + n_g;
    int m_sum = m_a + m_d + m_b + m_g;
    
    if (isOdd(n_sum) || isOdd(m_sum))
        return 0;
    
    // x = (n_a! m_a! n_d! m_d! n_b! m_b! n_g! m_g!)
    double x = 1;
    for (int n : {n_a, m_a, n_b, m_b, n_g, m_g, n_d, m_d})
        x *= factorial(n);
    
    // coeff = coefficient before sums, excluding any fixed prefactor
    double coeff = powerOfNegativeOne(n_b + m_b + n_g + m_g) / sqrt(x);

    // sum = sum of product of combinations and double factorial thingy
    double sum = 0;

    for (int p1=0; p1<=min(n_a,n_d); p1++) {
        double c1 = productOfCombinations(n_a, n_d, p1);
        
        for (int p2=0; p2<=min(m_a,m_d); p2++) {
            double c2 = productOfCombinations(m_a, m_d, p2);
            
            for (int p3=0; p3<=min(n_b,n_g); p3++) {
                double c3 = productOfCombinations(n_b, n_g, p3);
                
                for (int p4=0; p4<=min(m_b,m_g); p4++) {
                    double c4 = productOfCombinations(m_b, m_g, p4);
    
                    int a =  n_sum - 2*p1 - 2*p3;
                    int p = (m_sum - 2*p2 - 2*p4 + a)/2;

                    double term = (
                        c1 * c2 * c3 * c4 * powerOfNegativeOne(p) *
                        doubleFactorial(2*p-1) * doubleFactorial(2*p-a-1) * doubleFactorial(a-1) /
                        (pow(2, 2*p) * factorial(p)));
                        
                    sum += term;
                }
            }
        }
    }
    
    return coeff * sum;
}



void populateMatrix(double** matr, int maxInd) {
    int dim = (int) pow(maxInd, 4);
    
    // set matr[r][c] to <r|v|c>, where 
    //      <r| = <n_a, m_a, n_b, m_b|
    //      |c> = |n_g, m_g, n_d, m_d>
    // where the left-most index is least significant, and where threads
    // are given one or more rows
    
    #pragma omp parallel for
    for (int r=0; r<dim; r++) {
        
        array<int,8> inds;
        int rem;
        
        // map row to 4 indices
        rem = r;
        for (int i=0; i<4; i++) {
            inds[i] =rem % maxInd;
            rem /= maxInd;
        }
        
        for (int c=0; c<dim; c++) {
            
            // map column to 4 indices
            rem = c;
            for (int i=0; i<4; i++) {
                inds[4+i] =rem % maxInd;
                rem /= maxInd;
            }
            
            matr[r][c] = getIntegral(inds);
        }        
    }
}

void printMatrix(double** matr, int dim) {
    
    for (int r=0; r<dim; r++) {
        for (int c=0; c<dim; c++)
            printf("%.2f ", matr[r][c]);
        printf("\n");
    }
}

void saveMatrix(double** matr, int dim, char* fn) {
    
    ofstream file(fn);
    
    for (int r=0; r<dim; r++) {
        for (int c=0; c<dim; c++)
            file << matr[r][c] << " ";
        file << "\n";
    }
    
    file.close();
}


int main(int argc, char** argv) { 
    
    int numArgs = argc-1;
    
    if (numArgs != 8 && numArgs != 2) {
        printf("ERROR: expected 8 or 2 arguments\n");
        printf("Compute single element with: n_a m_a n_b m_b n_g m_g n_d m_d\n");
        printf("Compute full matrix with: ind_max filename\n");
        return 1;
    }
    
    if (numArgs == 8) {
        array<int,8> inds;
        for (int i=0; i<8; i++)
            inds[i] = atoi(argv[i+1]);
        double r = getIntegral(inds);
        printf("%g\n", r);
    }
    
    if (numArgs == 2) {
        int maxInd = atoi(argv[1]);
        char* fn = argv[2];
        
        int dim = (int) pow(maxInd, 4);
        double** matr = (double**) malloc(dim * sizeof *matr);
        for (int i=0; i<dim; i++)
            matr[i] = (double*) malloc(dim * sizeof **matr);
        
        populateMatrix(matr, maxInd);
        saveMatrix(matr, dim, fn);
        
        // free matrix heap memory
        for (int i=0; i<dim; i++)
            free(matr[i]);
        free(matr);
    }

    return 0;
}