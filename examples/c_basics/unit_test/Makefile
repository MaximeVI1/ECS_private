all: my_function.c  my_function.h	unit-test-my-function.c
	gcc -c my_function.c 
	gcc -Wall unit-test-my-function.c my_function.o -o unit-test-my-function 
# Wall enables extra warning messages from the compiler

clean: 
	$(RM) *.o unit-test-my-function
