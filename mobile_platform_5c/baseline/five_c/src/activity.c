/*
 *
 * @file activity.c
 * @brief Generic activity data structures and operators implementation
 *
 * This file contains the implementation of the generic activity.
 * It contains the
 * 1. data structures for
 *   - computation: activity state + configuration
 *   - life cycle management: state + configuration
 *   - activity structure models: DAG models
 * 2. generic function: do_activity ( alg_t * ) to execute an activity
 * 3. generic firing schedules (for firing lcsm events)
 *
 */

#include <five_c/activity/activity.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

void do_activity(activity_t *activity) {
    do_schedule_eventloop(&activity->schedule_table);
}

void update_super_state_lcsm_flags(lcsm_flags_t *lcsm_flags, const LCSM_state_t state){
    switch (state){
        case CREATION:    
            lcsm_flags->deployment = true;
            lcsm_flags->active = false;
            lcsm_flags->ready = false;
            break;
        case RESOURCE_CONFIGURATION:
            lcsm_flags->deployment = true;
            lcsm_flags->active = false;
            lcsm_flags->ready = false;
            break;
        case CAPABILITY_CONFIGURATION:
            lcsm_flags->deployment = false;
            lcsm_flags->active = true;
            lcsm_flags->ready = false;
            break;
        case PAUSING:
            lcsm_flags->deployment = false;
            lcsm_flags->active = true;
            lcsm_flags->ready = true;
            break;
        case RUNNING:
            lcsm_flags->deployment = false;
            lcsm_flags->active = true;
            lcsm_flags->ready = true;
            break;
        case CLEANING:
            lcsm_flags->deployment = true;
            lcsm_flags->active = false;
            lcsm_flags->ready = false;
            break;
        case DONE:
            lcsm_flags->deployment = false;
            lcsm_flags->active = false;
            lcsm_flags->ready = false;        
            break;    
    }
}

void create_lcsm_activity(activity_t *activity){
    ;
}

void configure_lcsm_activity(activity_t *activity, variable_table_t *table) {
     // LCSM initialization
    init_lcsm(&activity->lcsm);

    // LCSM flag configuration
    lcsm_flags_t *lcsm_flags = (lcsm_flags_t *) &activity->state.lcsm_flags;
    lcsm_flags->creation_complete = false;
    lcsm_flags->creation_failed = false;
    lcsm_flags->resource_configuration_complete = false;
    lcsm_flags->resource_configuration_failed = false;
    lcsm_flags->capability_configuration_complete = false;
    lcsm_flags->running_complete = false;
    lcsm_flags->deletion_complete = false;
    lcsm_flags->active = false;
    lcsm_flags->ready = false;
    lcsm_flags->deployment = false; 
    // Schedule table
    activity->schedule_table.number_of_schedules = 0;
    activity->schedule_table.number_of_eventloop_schedules = 0;
    // Variable table
    activity->table = table;
}

void destroy_activity(activity_t *activity) {
    /* Deletion conf structs */
    if (activity->conf.params)
        free(activity->conf.params);
 
    /* Deletion state structs */
    if (activity->state.computational_state.continuous)
        free(activity->state.computational_state.continuous);
    if (activity->state.computational_state.discrete)
        free(activity->state.computational_state.discrete);
    if (activity->state.coordination_state)
        free(activity->state.coordination_state);
}
