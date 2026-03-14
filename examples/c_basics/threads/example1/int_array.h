#ifndef INT_ARRAY_H
#define INT_ARRAY_H

#include <stdlib.h>

typedef struct int_array_s{
    int *values;    // pointer to an array of ints
    size_t length;  // number of integers in the array
} int_array_t;

void create_int_array(int_array_t *array); // are all declared in int_array.c

void allocate_memory_int_array(int_array_t *array, size_t length);

void deallocate_memory_int_array(int_array_t *array);

#endif