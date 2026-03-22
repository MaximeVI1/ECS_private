/*
 *
 * @file schedule.h
 * @brief Header of 
 *
 * This file is the header of the implementation of 
 *
 * (c) Filip Reniers (KU Leuven) 27/10/2020
 *
 */

#ifndef DEPLOYMENT_ARCHITECTURE_SCHEDULE_H
#define DEPLOYMENT_ARCHITECTURE_SCHEDULE_H

#include <uthash.h>
#include <stdbool.h>

#define MAX_LENGTH_FUNCTION_NAME 64
#define MAX_LENGTH_SCHEDULE_NAME 64

#define MAX_NUMBER_OF_FUNCTIONS 16
#define MAX_NUMBER_OF_SCHEDULES 32

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*function_ptr_t)(void *);
typedef bool (*expression_ptr_t)(void *);

//! Dats structure for generic function handler
typedef struct named_function_s { 
    char name[MAX_LENGTH_FUNCTION_NAME];  ///< Name of the function 
    function_ptr_t function;  ///< pointer for the function
    void *arguments; ///< arguments of the function 
} named_function_t;

//! Data structure for a schedule - set of functions (function pointer, arguments) 
typedef struct schedule_s { 
    char name[MAX_LENGTH_SCHEDULE_NAME];  ///< Name to unqieuly identify a schedule
    named_function_t named_functions[MAX_NUMBER_OF_FUNCTIONS]; //< Array of functions to be executed in a schedule
    int number_of_functions; ///< number of entries in a schedule
} schedule_t;

//! Data structure for a hashable schedule (using uthash library)
typedef struct hashable_schedule_s {
    char name[MAX_LENGTH_SCHEDULE_NAME];  ///< key (string is WITHIN the structure) 
    schedule_t schedule;
    UT_hash_handle hh;  ///< makes this structure hashable 
} hashable_schedule_t;

//! Data structure for a schedule table (all schedules + eventloop)
typedef struct schedule_table_s { 
    hashable_schedule_t *hash_map;
    schedule_t eventloop_schedules[MAX_NUMBER_OF_SCHEDULES]; ///< Array of schedules
    int number_of_schedules; ///< number of schedules in the array
    int number_of_eventloop_schedules; ///< number of schedules in the array
} schedule_table_t;

//! Register function to a schedule
void register_function(schedule_t *schedule, function_ptr_t function_ptr, 
    void *function_args, char *function_name);

//! Register schedule to a schedule table
void register_schedule(schedule_table_t *schedule_table, schedule_t schedule, 
    char *schedule_name);

//! Adds a schedule to be executed in the eventloop
void add_schedule_to_eventloop(schedule_table_t *schedule_table, 
    char *schedule_name);

//! Removes a schedule from the eventloop
void remove_schedule_from_eventloop(schedule_table_t *schedule_table, 
   char *schedule_name);
   
//! Executes the schedules that are in the eventloop
void do_schedule_eventloop(schedule_table_t *schedule_table);

//! Execute functions registered in a schedule
void execute_schedule(schedule_t *schedule);

#ifdef __cplusplus
}
#endif

#endif //DEPLOYMENT_ARCHITECTURE_SCHEDULE_H
