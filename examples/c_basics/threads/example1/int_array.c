#include "int_array.h"

void create_int_array(int_array_t *array){
    array->length = 0;
    array->values = NULL;
}

void allocate_memory_int_array(int_array_t *array, size_t length){
    array->values = (int *) calloc(length, sizeof(int));
    if (array->values != NULL){
        array->length = length;
    }else{
        array->length = 0;
    }
}

void deallocate_memory_int_array(int_array_t *array){
    if (array->values != NULL){
        free(array->values);
        array->values = NULL;
        array->length = 0;
    }
}
