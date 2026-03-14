
#include <stdio.h>
#include <string.h>

#include <lapacke.h>
#include <cblas.h>

//! Print matrix to the console, but it doesnt print?
void print_dmatrix(const double *matrix, int nrows,
                   int ncolumns);

int main(int argc, const char **argv)
{
   printf("-------------------------\n");
   printf("Solving Ax = b\n");
   printf("-------------------------\n");
   // Matrix A
   double A[4] = {2, 1, 1, 1}; // double when need of higher precision then float
   printf("Matrix A (coefficients of the system):\n");
   print_dmatrix(A, 2, 2);

   // Vector b
   double b[2] = {3, 1};
   printf("Vector b (constant terms):\n");
   print_dmatrix(b, 2, 1);

   // Let's solve the problem by computing x = A_inv * b
   // First, compute the inverse of A, A_inv.
   double A_inv[4];
   memcpy((void *)A_inv, (void *)A, 4 * sizeof(double));

   lapack_int IPIV[4];
   lapack_int A_nrows = 2, A_ncolums = 2, lda = 2;
   lapack_int info;

   // dgetrf (General Matrix Factorization) Computes matrix LU such
   // that A = PLU, where L is a lower triangular and U is upper triangular.
   // <https://www.ibm.com/docs/en/essl/6.2?topic=subroutines-sgetrf-dgetrf-cgetrf-zgetrf-general-matrix-factorization>
   info = LAPACKE_dgetrf(LAPACK_ROW_MAJOR, // matrix_layout
                         A_nrows,
                         A_ncolums,
                         A_inv,
                         lda,
                         IPIV);

   // dgetri (General Matrix Inverse)
   // <https://www.ibm.com/docs/en/essl/6.1?topic=dlaes-sgetri-dgetri-cgetri-zgetri-sgeicd-dgeicd-general-matrix-inverse-condition-number-reciprocal-determinant>
   info = LAPACKE_dgetri(LAPACK_ROW_MAJOR,
                         A_nrows,
                         A_inv,
                         lda,
                         IPIV);

   if (info > 0)
   {
      printf("Matrix of coefficients is singular!\n");
      return EXIT_FAILURE;
   }
   printf("A_inv:\n");
   print_dmatrix(A_inv, 2, 2);

   double alpha = 1, beta = 0;
   double AA_inv[4];
   // dgemm stands for "Combined Matrix Multiplication and Addition for
   // General Matrices, Their Transposes, or Conjugate Transposes".
   // For matrix A,B, and C: C = alpha*A*B + beta*C, where alpha and beta
   // are scalars.
   // <https://www.ibm.com/docs/en/essl/6.3?topic=mos-sgemm-dgemm-cgemm-zgemm-combined-matrix-multiplication-addition-general-matrices-their-transposes-conjugate-transposes>
   cblas_dgemm(CblasRowMajor,
               CblasNoTrans,
               CblasTrans,
               A_nrows,
               A_ncolums,
               A_ncolums,
               alpha,
               A,
               lda,
               A_inv,
               lda,
               beta,
               AA_inv,
               lda);

   printf("A * A_inv: \n");
   print_dmatrix(AA_inv, 2, 2);

   // Vector x that will hold the result
   double x[2] = {0, 0};
   // dgemv stands for "Matrix-Vector Product for a General Matrix, Its
   // Transpose, or Its Conjugate Transpose"
   // For matrix A, vectors x and y: y = beta*y + alpha*A*x, where alpha
   // and beta are scalars.
   // <https://www.ibm.com/docs/en/essl/6.3?topic=mvs-sgemv-dgemv-cgemv-zgemv-sgemx-dgemx-sgemtx-dgemtx-matrix-vector-product-general-matrix-its-transpose-its-conjugate-transpose>
   alpha = 1;
   beta = 0;
   cblas_dgemv(CblasRowMajor,
               CblasNoTrans,
               A_nrows,
               A_ncolums,
               alpha,
               A_inv,
               lda,
               b,
               1, // incx,
               beta,
               x,
               1 // incy
   );

   printf("x = A_inv * b: \n");
   print_dmatrix(x, 2, 1);

   printf("%d\n", argc);

   return (EXIT_SUCCESS);
}

void print_dmatrix(const double *matrix, int nrows, int ncolumns)
{
   for (int i = 0; i < nrows; i++)
   {
      for (int j = 0; j < ncolumns; j++)
      {
         if (ncolumns > 1)
         {
            printf("%lf ", matrix[i * nrows + j]);
         }
         else
         {
            printf("%lf (%d)", matrix[i], i);
         }
      }
      printf("\n");
   }
}
