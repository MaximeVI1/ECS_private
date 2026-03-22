/*
 *
 * @file activity.h
 * @brief Header of generic activity data structures
 *
 * This file is the header of the implementation of an activity component.
 * It contains the
 * 1. data structures for
 *   - computation: activity state + configuration
 *   - life cycle management: state + configuration
 *   - activity structure models: DAG models
 * 2. generic function: do_activity ( alg_t * ) to execute an activity
 * 3. generic firing schedules (for firing lcsm events)
 */

#ifndef FIVE_C_ACTIVITY_H
#define FIVE_C_ACTIVITY_H

#include <stdbool.h>

#include <coordination_libraries/lcsm/lcsm.h>
#include <coordination_libraries/fsm/FSM.h>
#include <coordination_libraries/petrinet/petrinet.h>

#include <data_structure/registration/hash_table_variable.h>

#include <five_c/schedule/schedule.h>
#include <five_c/scheduler/eventloop_composition_and_execution/petrinet_scheduler.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {DEPLOYMENT, ACTIVE, READY} lcsm_superstate_t;
typedef enum {INITIALISATION, EXECUTION, DEINITIALISATION} lcsm_protocol_t;

// CONFIGURATION DATA STRUCTURE
typedef struct activity_conf_s {
    void *params;
} activity_conf_t;

// STATE DATA STRUCTURE
// common life-cycle management flags
typedef struct lcsm_flags_s {
    bool creation_complete;
    bool creation_failed;
    bool resource_configuration_complete;
    bool resource_configuration_failed;
    bool capability_configuration_complete;
    bool capability_configuration_failed;
    bool running_complete;
    bool deletion_complete;
    bool active;
    bool ready;
    bool deployment;
} lcsm_flags_t;

// Computational state 
typedef struct computational_state_s {
    void *discrete; // status of computations
    void *continuous;
} computational_state_t;

// Complete state 
typedef struct activity_state_s {
    computational_state_t computational_state;
    void *coordination_state;
    lcsm_flags_t lcsm_flags;
    lcsm_protocol_t lcsm_protocol;
    flag_token_conversion_map_t *petrinet_flag_map;
} activity_state_t;

// ACTIVITY DATA STUCTURE
typedef struct activity_s {
    unsigned long mid;
    struct timespec *clocktime;
    LCSM_t lcsm;
    FSM_t *fsm;
    petrinet_t *petrinet;
    activity_conf_t conf;
    activity_state_t state;
    schedule_table_t schedule_table;
    variable_table_t *table;
}activity_t;

typedef struct activity_lcsm_s{
    void (*create)(activity_t *, const char *name);
    void (*configure)(activity_t *, variable_table_t *);
    void (*destroy)(activity_t *);
}activity_lcsm_t;

void do_activity(activity_t *activity);
void update_super_state_lcsm_flags(lcsm_flags_t *lcsm_flags, const LCSM_state_t state);
void create_lcsm_activity(activity_t *activity);
void configure_lcsm_activity(activity_t *activity, variable_table_t *table);
void destroy_activity(activity_t *activity);

#ifdef __cplusplus
}
#endif

#endif //FIVE_C_ACTIVITY_H

