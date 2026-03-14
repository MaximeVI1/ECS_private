# C examples
## Installing requirements:
`sudo apt-get install build-essential`

Build-essential contains the GCC (GNU C compiler)

`sudo apt-get install libblas-dev liblapacke-dev libatlas-base-dev`

BLAS and LAPACK support linear algebra operations

## Exploring
The suggested order to explore these C examples is

1. Data structure
2. Pointers
3. Linear algebra
4. Threads
5. Unit tests

The examples are built using Makefiles.
In each folder there is at least a .c and a Makefile.
To compile an example, navigate to the example folder and type

`make`

This will create an executable. To run the executable, enter

`./<name-of-the-executable>`

Try to understand the Makefile:
- what is the name of the executable?
- How to compile multiple files?
- How to link libraries?  

References:

[Make](https://en.wikipedia.org/wiki/Make_(software)#Makefile)

[Beej's Guide to C Programming](https://beej.us/guide/bgc/html/split/index.html)

[LAPACK](https://netlib.org/lapack/)

[CBLAS IBM](https://www.ibm.com/docs/en/essl/6.2?topic=reference-basic-linear-algebra-subprograms-blas-blas-cblas)

[LAPACKE IBM](https://www.ibm.com/docs/en/essl/6.2?topic=reference-lapack-lapacke)

[assert.h](https://en.wikipedia.org/wiki/Assert.h)
