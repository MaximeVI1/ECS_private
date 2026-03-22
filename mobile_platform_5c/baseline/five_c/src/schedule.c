/*
 *
 * @file schedule.c
 * @brief 
 *
 * This file contains the implementation of 
 *
 * (c) Filip Reniers (KU Leuven) 27/10/2020
 *
 */

#include <five_c/schedule/schedule.h>

#include <string.h>
#include <stdio.h>

/**
 * Register a named function to a schedule and updates the number of 
 * functions in the schedule.
 * @param[in] schedule: pointer to the schedule where the function 
 * shall be registered to
 * @param[in] function_ptr function itself
 * @param[in] function_args a pointer to the arguments of the function
 * @param[in] function_name a name to the function (currently, it does 
 * not need to be unique). 
 * 
 * @NOTE: Does it make sense to have a function_name if it does not need 
 * to be unique? The functions are always executed in the order which they 
 * are registered in the schedule.
 * */
void register_function(schedule_t *schedule, function_ptr_t function_ptr, void *function_args, char *function_name){
    snprintf(schedule->named_functions[schedule->number_of_functions].name, MAX_LENGTH_FUNCTION_NAME, "%s", function_name);
    schedule->named_functions[schedule->number_of_functions].function = function_ptr;
    schedule->named_functions[schedule->number_of_functions].arguments = function_args;
    schedule->number_of_functions += 1;
}

/**
 * Register a schedule to a schedule table. The schedule at this point must be 
 * completed, that is, the functions have already been registered to the 
 * schedule that is being added to the schedule table. This functions updates 
 * the number of the schedule registered in the schedule table.
 * @param[in] schedule_table  pointer to the schedule table where the schedule 
 * shall be registered to
 * @param[in] schedule_table schedule to be registered
 * @param[in] schedule_name name of a sch
 * @param[in] function_name a name to uniquely identify the function in a schedule
 * */
void register_schedule(schedule_table_t *schedule_table, schedule_t schedule, char *schedule_name){
    hashable_schedule_t *hashable_schedule = (hashable_schedule_t *) malloc(sizeof(hashable_schedule_t));
    hashable_schedule->schedule = schedule;  // This is the actual schedule which will be execute
    // The name of the schedule is overwritten to ensure that the hash value and the schedule name
    // are always concise.
    snprintf(hashable_schedule->name, MAX_LENGTH_SCHEDULE_NAME, "%s", schedule_name);
    snprintf(hashable_schedule->schedule.name, MAX_LENGTH_SCHEDULE_NAME, "%s",schedule_name);
    HASH_ADD_STR( schedule_table->hash_map, name, hashable_schedule );

    schedule_table->number_of_schedules += 1;
}

/**
 * This function is an interface with uthash library. It searches for an entry 
 * in the schedule map for which the key is the schedule_name
 * @param[in] hash_map the hashable map where the search takes place
 * @param[in] schedule_name the string to be searched in the hash map
 * @param[out] hashable_schedule a pointer to a schedule if there is a match, 
 * or a null pointer if the entry does not existe in the schedule map
 *  */
hashable_schedule_t *find_schedule_map_entry(hashable_schedule_t **hash_map, char *schedule_name){
    hashable_schedule_t *hashable_schedule;
    HASH_FIND_STR( *hash_map, schedule_name, hashable_schedule);
    return hashable_schedule;
}

/**
 * Add a schedule (which has been previously registered) to the eventloop. 
 * Schedules in the eventloop will be executed during a do_eventloop() run.
 * @param[in] schedule_table  pointer to the schedule table that cointains the
 * schedule to be added in the eventloop. 
 * @param[in] schedule_name the string that corresponds to the name of the 
 * schedule that one wants to add in the eventloop.
 * 
 * Nothing happens if the name of the schedule is not found the schedule_table.
 * @TODO: Implement a mechanism that prevents a schedule to be added more than
 * one time to the eventloop.
 * */
void add_schedule_to_eventloop(schedule_table_t *schedule_table, char *schedule_name) {
    hashable_schedule_t *hash_entry = find_schedule_map_entry(&schedule_table->hash_map, schedule_name);
    if (hash_entry){
        if (&hash_entry->schedule) {
            schedule_table->eventloop_schedules[schedule_table->number_of_eventloop_schedules] = hash_entry->schedule;
            schedule_table->number_of_eventloop_schedules++;
        }
    }
}

/**
 * Remove a schedule from the eventloop. Complementary to add_schedule_to_eventloop()
 * @param[in] schedule_table  pointer to the schedule table that cointains the
 * schedule to be removed from the eventloop. 
 * @param[in] schedule_name the string that corresponds to the name of the 
 * schedule that one wants to remove from the eventloop.
 * 
 * Nothing happens if the schedule does not exist in the event loop
 * */
void remove_schedule_from_eventloop(schedule_table_t *schedule_table, char *schedule_name) {
    for (int i = 0; i < schedule_table->number_of_eventloop_schedules; i++) {
        // Find schedule
        if (strcmp(schedule_table->eventloop_schedules[i].name, schedule_name) == 0){
            int last_index = schedule_table->number_of_eventloop_schedules - 1;
            // Check whether it is the last element of the array
            if (i != last_index) {
                // shift elements forward to fill gap in array
                for (int j = i; j < last_index; j++ ) {
                    schedule_table->eventloop_schedules[j] = schedule_table->eventloop_schedules[j + 1];
                }
            }
            //schedule_table->eventloop_schedules[last_index] = NULL;
            // decrement array size
            schedule_table->number_of_eventloop_schedules--;
        }
    }
}

/**
 * Executes a name function 
 * @param[in] func pointer to a funtion to be executed
 * */
void execute_named_function(named_function_t *func){
    func->function(func->arguments);
}

/**
 * Executes a all the functions that have been registered to a schedule. The 
 * funtions are executed in the same order in which they were registered. 
 * @param[in] schedule pointer to a schedule to be executed
 * */
void execute_schedule(schedule_t *schedule){
    for (int i = 0; i < schedule->number_of_functions; i++){
        execute_named_function(&schedule->named_functions[i]);
    }
}

/**
 * Executes a all the schedules that are in eventloop of schedule_table. The 
 * schedules are executed in the order in which they were added to the 
 * eventloop. 
 * @param[in] schedule_table pointer to a schedule table to be executed
 * */
void do_schedule_eventloop(schedule_table_t *schedule_table) {
    for(int i = 0; i < schedule_table->number_of_eventloop_schedules; i++) {
        execute_schedule(&schedule_table->eventloop_schedules[i]);
    }
}

// ---------------------------------------------------------------------------- //
// Do we need the functions below? If yes, we probably need to re-implement
// them to the new schedule infrastructure
// --------------------------------------------------------------------------- //
void clear_schedule(schedule_table_t *schedule_table){
    schedule_table->number_of_eventloop_schedules = 0;
}

void remove_schedule_map_entry(hashable_schedule_t **hash_map, hashable_schedule_t *schedule_map_entry){
    HASH_DEL(*hash_map, schedule_map_entry);  /* user: pointer to deletee */
}

void delete_schedule_map_entry_completely(hashable_schedule_t **hash_map, hashable_schedule_t *schedule_map_entry){
    HASH_DEL(*hash_map, schedule_map_entry);  /* user: pointer to deletee */
    free(schedule_map_entry);
}

void delete_whole_schedule_map(hashable_schedule_t **hash_map){
    hashable_schedule_t *current_entry, *tmp;
    HASH_ITER(hh, *hash_map, current_entry, tmp) {
        HASH_DEL(*hash_map, current_entry);
        free(current_entry);
    }
}
