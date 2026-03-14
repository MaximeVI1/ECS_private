/**
 * Example of data sharing between thread for 
 * keyboard control of linear velocity.
 * 
 * @file: keyboard_interface.h
 */

// Generic C headers
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <signal.h>
#include <pthread.h>
#include <unistd.h>


// Include helper functions for keyboard interface
#include <keyboard.h>

// Define shared variable for counter
int counter;

// Define flag to indicate when shutdown is needed
bool shutdown;

// Define functions implemented in the main file
void *print_counter(void *arg);
void *keyboard_listen(void *arg);