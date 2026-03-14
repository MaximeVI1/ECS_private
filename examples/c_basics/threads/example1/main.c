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
 * the values of an integer array are incremented from 0 to NUMBER_OF_ITERATIONS 
 * (constant defined above), at unit step. After each increment, the function 
 * "sleeps" for SLEEPING_TIME ms (constant defined above). Concurrently, in another 
 * thread (thread_for_printing), the values of the array are printed to the 
 * console also for NUMBER_OF_ITERATIONS times.
 * 
 * Behaviour: The variable "samples" of type int_array_t has 20 elements and it is shared 
 * by both threads. This means that both threads are able to _read_ and _write_ in the same 
 * region of the memory, in that case, the area corresponding to the "samples" variable. 
 * The increment function updates all the values of the array by one unit in a for() loop. 
 * The printing function prints to the console all the values of the array. Do you expect 
 * the printed values of the array to be always the same within the same printing statement?
 * Run this executable a few times and see if you observe anything unexpected. 
 * 
 * Is it possible to conclude which one is fastest: the printing or the 
 * iteration function?
 * 
 * Students are reccomend to read the thread section in the reference book
 * <https://beej.us/guide/bgc/html/split/multithreading.html#multithreading>
 */
int main(int argc, char **argv){
    int_array_t samples;

    create_int_array(&samples);
    allocate_memory_int_array(&samples, 20);

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
        // Update all the values of the array to "num"
        for(size_t i=0; i<array->length; i++){
            array->values[i] = num;
        }   
        sleep(SLEEPING_TIME);
    }
    return 0;
}

int print_int_array(void *args){
    // cast void pointer into int_array_t pointer
    int_array_t *array = (int_array_t *) args; 

    int num = 0;
    while (num++ < NUMBER_OF_ITERATIONS){
        for(size_t i=0; i<array->length; i++){
            printf("%d ", array->values[i]);
        }   
        printf("\n");
    }
    return 0;
}
