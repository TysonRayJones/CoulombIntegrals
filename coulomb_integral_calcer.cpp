// supply cmd-line arg -DUSE_CACHE for a speedup

#include <math.h>
#include <stdio.h>
#include <cmath>
#include <initializer_list>
#include <array>


using std::array;
using std::ofstream;


#define CACHE_SIZE 200
double factorialCache[CACHE_SIZE];
double doubleFactorialCache[CACHE_SIZE];


void prepare_caches(int maxInd) {

    #ifdef USE_CACHE
        
        int maxFacInput = 8*maxInd;
        if (maxFacInput > CACHE_SIZE) {
            printf("Cache size too small; increase to %d\n", maxFacInput);
            exit(1);
        }
        
        factorialCache[0] = 1;
        double f = 1;
        int i;
        for (i=1; i<maxFacInput; i++) {
            f *= i;
            factorialCache[i] = f;
        }
        factorialCache[i] = -1;
        
        doubleFactorialCache[0] = 1;
        f = 1;
        for (i=1; i<maxFacInput; i+=2) {
            f *= i;
            doubleFactorialCache[i] = f;
        }
        doubleFactorialCache[i] = -1;
        
    #endif
}

void print_caches() {
    #ifdef USE_CACHE
        for (int i=0; factorialCache[i] != -1; i++) {
            printf("%d! = %g", i, factorialCache[i]);
            if (i%2)
                printf("\t\t%d!! = %g", i, doubleFactorialCache[i]);
            printf("\n");
        }
    #else
        printf("No caches prepared (USE_CACHE=0)");
    #endif
}



double inline factorial(int n) {
    #ifdef USE_CACHE
        return factorialCache[n];
    #endif
    
    
    // double to avoid int overflow
    double f = 1;
    while (n > 1)
        f *= n--;
    return f;
}

double inline doubleFactorial(int n) {
    #ifdef USE_CACHE
        return doubleFactorialCache[n];
    #endif
    
    // double to avoid int overflow
    double f = 1;
    while (n > 1) {
        f *= n;
        n -= 2;
    }
    return f;
}

double inline combinations(int n, int r) {
    #ifdef USE_CACHE
        if (r >= n)
            return 0;
        return factorial(n)/(factorial(r) * factorial(n-r));
    #endif
    
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

double inline getIntegral(array<int,8> inds) {
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
    
    FILE* file = fopen(fn, "w");
    
    for (int r=0; r<dim; r++) {
        for (int c=0; c<dim; c++)
            fprintf(file, "%g ", matr[r][c]);
        fprintf(file, "\n");
    }
    
    fclose(file);
}




void populateRow(double* row, int r, int maxInd) {
    int dim = (int) pow(maxInd, 4);
    array<int,8> inds;
    
    // map row to 4 indices
    int rem = r;
    for (int i=0; i<4; i++) {
        inds[i] =rem % maxInd;
        rem /= maxInd;
    }
    
    // iterate every column
    for (int c=0; c<dim; c++) {
        
        // map column to 4 indices
        int rem = c;
        for (int i=0; i<4; i++) {
            inds[4+i] =rem % maxInd;
            rem /= maxInd;
        }
        
        row[c] = getIntegral(inds);
    }     
}

void saveRow(FILE* file, double* row, int dim) {
    for (int c=0; c<dim; c++)
        fprintf(file, "%g ",row[c]);
    fprintf(file, "\n");
}



int main(int argc, char** argv) { 
    
    int numArgs = argc-1;
    
    if (numArgs != 8 && numArgs != 2) {
        printf("ERROR: expected 8 or 2 arguments\n");
        printf("Compute single element with: n_a m_a n_b m_b n_g m_g n_d m_d\n");
        printf("Compute full matrix with: ind_max filename\n");
        printf("Compile with -DUSE_CACHE to accelerate simulation via caches\n");
        printf("Compile with -fopenmp to accelerate simulation with multithreading (can use caching too)\n");
        printf("Compile without -fopenmp to serially add rows to file (small memory overhead)\n");
        return 1;
    }
    
    if (numArgs == 8) {
        array<int,8> inds;
        for (int i=0; i<8; i++)
            inds[i] = atoi(argv[i+1]);
            
        int maxInd = -1;
        for (int i=0; i<8; i++)
            if (inds[i] > maxInd)
                maxInd = inds[i];
            
        prepare_caches(maxInd);
        print_caches();
        
        double r = getIntegral(inds);
        printf("%g\n", r);
    }
    
    if (numArgs == 2) {
        int maxInd = atoi(argv[1]);
        char* fn = argv[2];
        
        prepare_caches(maxInd);
        int dim = (int) pow(maxInd, 4);
        
        // if multithreaded, construct entire matrix in memory
        # ifdef _OPENMP
            printf("Multithreaded mode. Constructing entire matrix in memory...");
            
            double** matr = (double**) malloc(dim * sizeof *matr);
            for (int i=0; i<dim; i++)
                matr[i] = (double*) malloc(dim * sizeof **matr);
            
            populateMatrix(matr, maxInd);
            saveMatrix(matr, dim, fn);
            
            // free matrix heap memory
            for (int i=0; i<dim; i++)
                free(matr[i]);
            free(matr);
            
        // if serial, append row by row to file
        #else
            printf("Serial mode. Writing to file row by row...\n");
        
            FILE* file = fopen(fn, "w");
            double* row = (double*) malloc(dim * sizeof *row);
            
            for (int r=0; r<dim; r++) {
                populateRow(row, r, maxInd);
                saveRow(file, row, dim);
            }
            
            fclose(file);
        
        #endif
    }

    return 0;
}