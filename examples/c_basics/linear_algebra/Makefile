all: ex1_lapacke_and_blas.c ex2_lapacke.c
	gcc ex1_lapacke_and_blas.c -lcblas -llapacke -o ex1 
# als je extra niet direct geinstalleerde libraries gebruikt moet je deze ook meegeven aan de compiler
	gcc ex2_lapacke.c -llapacke -o ex2
clean: 
	$(RM) ex1 ex2
