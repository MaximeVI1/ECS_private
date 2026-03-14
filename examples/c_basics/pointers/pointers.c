#include <stdio.h> // printf
#include <stdlib.h> // malloc, exit constants 

/**
 * This a tutorial about pointer. Mastering pointers in C is an
 * important skill for sailing through ECS in a safe way,
 * that is, free from so-called "segmentation faults"
 * <https://en.wikipedia.org/wiki/Segmentation_fault>.
 * 
 * Make sure you have read the relevant section in the reference book: 
 * <https://beej.us/guide/bgc/html/split/pointers.html#pointers>
 * 
 * Use this file as playground to try different things with pointers,
 * e.g., create an array with n elements and compute the sum of the 
 * elements in the array.
 */

int main(int argc, char **argv)
{
  // Here we define a variable of type int. The compiler will 
  // allocate memory for an int type, (4 bytes in 64 architecture)
  int my_int_value;
  
  // When definig a variable, the operator * refers to a 
  // pointer, for example
  int* my_int_pointer;  // or int *my_int_pointer;
  // defines a pointer of type int (8 bytes in 64 architecture)
  // The asterix can be placed just after the data type or just
  // before the variable name.

  /*### SIZE OF VARIABLES ###*/
  // The function sizeof returns the number of bytes allocated
  // for a variable
  printf("number of bytes of int: %ld\n", sizeof(my_int_value));
  printf("number of bytes of int*: %ld\n", sizeof(my_int_pointer));
  
  /*### Memory allocation ###*/
  // <https://en.wikipedia.org/wiki/C_dynamic_memory_allocation>
  // We need to allocate memory for the int pointer. We can use
  // malloc or calloc (homework: what is the difference between them?)
  // malloc: <https://beej.us/guide/bgc/html/split/manual-memory-allocation.html#allocating-and-deallocating-malloc-and-free>
  // calloc: <https://beej.us/guide/bgc/html/split/manual-memory-allocation.html#an-alternative-calloc>
  my_int_pointer = (int *) malloc(sizeof(int)*1);
  // malloc returns a void pointer, which is casted into an int pointers
  // using the instruction (int *). More about casting in
  // <https://beej.us/guide/bgc/html/split/types-iii-conversions.html#casting>
  // the argument of malloc is the number of bytes to be allocated. In this
  // example, we are allocating memory for one integer.

  // If malloc fails to allocate memory, a NULL pointer is returned
  if (my_int_pointer == NULL){
    // Failed to allocate memory
    return EXIT_FAILURE;
    // Exit MACROS are defined in stdlib.h
    // <https://beej.us/guide/bgc/html/split/the-outside-environment.html#exit-status>
  }

  /*### Assigning values ###*/
  my_int_value = 5;
  *my_int_pointer = 6;  // equivalent to my_int_pointer[0] = 6;
  // see that to assign (or access) a value of a pointer we need first
  // to dereference the pointer using the operator *. More about it:
  // <https://beej.us/guide/bgc/html/split/pointers.html#deref>

  // my_int_pointer = 6; is wrong because the left side operand is a 
  // memory address.

  /*### Printing values ###*/
  printf("my_int_value: %d\n", my_int_value);
  printf("*my_int_pointer: %d\n", *my_int_pointer);

  // It is a good practice to free the memory and set the pointers to NULL
  free(my_int_pointer);
  my_int_pointer = NULL;

  return EXIT_SUCCESS;
}


