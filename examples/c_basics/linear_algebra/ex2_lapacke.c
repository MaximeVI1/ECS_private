
#include <stdio.h>
#include <string.h>

#include <lapacke.h>
#include <cblas.h>

//! Print matrix to the console
void print_dmatrix(const double *matrix, int nrows, 
   int ncolumns);

int main (int argc, const char **argv)
{
    printf("-------------------------\n");
    printf("Solving Ax = b\n");
    printf("-------------------------\n");
    // Matrix A
    double A[4] = {2,1,1,1};
    printf("Matrix A (coefficients of the system):\n");
    print_dmatrix(A, 2, 2);

    // Vector b
    double b[2] = {3,1};
    printf("Vector b (constant terms):\n");
    print_dmatrix(b, 2, 1);

    // First, compute the inverse of A, A_inv.
    lapack_int info, A_nrows, A_ncolums, B_ncolumns, lda, ldb;
    A_nrows = 2;
    A_ncolums = 2;
    B_ncolumns = 1;
    lda = 2;
    ldb = 1;

    // dgels stands for "Linear Least Squares Solution for a General Matrix"
    // <https://www.ibm.com/docs/en/essl/6.1?topic=llss-sgels-dgels-cgels-zgels-linear-least-squares-solution-general-matrix>
    info = LAPACKE_dgels(LAPACK_ROW_MAJOR,
        'N',
        A_nrows,
        A_ncolums,
        B_ncolumns,
        A,
        lda,
        b,
        ldb);
   printf("x = A_inv * b: \n");
   print_dmatrix(b, 2, 1);

   return(EXIT_SUCCESS);
}

void print_dmatrix(const double *matrix, int nrows, int ncolumns){
    for(int i=0;i<nrows;i++){
        for(int j=0;j<ncolumns;j++){
            if(ncolumns>1){
                printf("%lf ", matrix[i*nrows+j]);
            }else{
                printf("%lf (%d) ", matrix[i], i);
            }
        }  
        printf("\n");
    }
}
