#include<threads.h>
#include<stdio.h>   // printf
#include<unistd.h>  // sleep

#include "int_array.h"

#define NUMBER_OF_ITERATIONS 100  // number of times array elements have to be incremented
#define SLEEPING_TIME 0.001 // sleep time in ms after each update of array elements    

// functions are declared here, but defined after the main()
int increment_int_array(void *args);

int print_int_array(void *args);

/**
 * This program launches two threads. In one thread (thread_for_incrementing),
 * the values of an integer array are incremented from 0 to 100 (step: 1). After
 * each increment, the function "sleeps" for 1 ms. Concurrently, in another 
 * thread (thread_for_printing), the values of the array are printed to the 
 * console a 100 times.
 * 
 * To ensure consistency and avoid racing conditions, we have added mutex to 
 * control reading/writing access of the variable samples (type int_array_t). 
 * 
 * Students are reccomend to read the mutex section in the reference book
 * <https://beej.us/guide/bgc/html/split/multithreading.html#mutex>
 */

mtx_t mutex;    

int main(int argc, char **argv){
    int_array_t samples;
    create_int_array(&samples);
    allocate_memory_int_array(&samples, 20);

    // Initialise mutex
    mtx_init(&mutex, mtx_plain);
    // read more about mutex initialisation in 
    // <https://en.cppreference.com/w/c/thread/mtx_init>

    thrd_t thread_for_incrementing, thread_for_printing;     
    printf("Launching threads\n");
    thrd_create(&thread_for_printing, print_int_array, &samples);
    thrd_create(&thread_for_incrementing, increment_int_array, &samples);

    printf("Waiting for threads to complete...\n");
    int res_increment, res_print;  // Holds return value from the thread exit
    // Wait here for the thread to complete; store the return value
    // in res:
    thrd_join(thread_for_printing, &res_print);
    thrd_join(thread_for_incrementing, &res_increment);

    printf("Thread for incrementing  exited with return value %d\n", 
        res_print);
    printf("Thread for printing exited with return value %d\n", 
        res_increment);

    deallocate_memory_int_array(&samples);
    printf("Memory deallocated\n");

    return 0;
}

int increment_int_array(void *args){
    int_array_t *array = (int_array_t *) args; // cast void pointers

    int num = 0;
    while (num++ < NUMBER_OF_ITERATIONS){
        // Update all the values of the array
        mtx_lock(&mutex);           // <-- ACQUIRE MUTEX
        for(size_t i=0; i<array->length; i++){
            array->values[i] = num;
        }   
        mtx_unlock(&mutex);
        sleep(SLEEPING_TIME);
    }
    return 0;
}

int print_int_array(void *args){
    int_array_t *array = (int_array_t *) args; // cast void pointers

    int num = 0;
    while (num++ < NUMBER_OF_ITERATIONS){
        mtx_lock(&mutex);           // <-- ACQUIRE MUTEX
        for(size_t i=0; i<array->length; i++){
            printf("%d ", array->values[i]);
        }   
        printf("\n");
        mtx_unlock(&mutex);
    }
    return 0;
}
