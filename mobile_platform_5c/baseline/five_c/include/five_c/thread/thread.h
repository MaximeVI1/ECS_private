#ifndef THREAD_H
#define THREAD_H

#include <stdbool.h>
#include <unistd.h>
#include <five_c/schedule/schedule.h>
#include <five_c/activity/activity.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_NB_ACTIVITIES 16

// Thread data structure
typedef struct thread_s {
    schedule_t schedule; 
    struct timespec clocktime;   
    useconds_t cycle_time_us;   ///< Thread cycle time in microseconds
    int nb_activities;  ///< Number of activities registered in a thread's schedule
    bool *activities_deletion_complete[MAX_NB_ACTIVITIES];  ///< Flags for monitoring activities' alive state
}thread_t;

//! Executes activities registered in the thread's schedule. 
void* do_thread_loop(void *thread_casted_void);

//! Properly initialises thread's data structure
void create_thread(thread_t *thread, char *thread_name, long int cycle_time_ms);

//! Register an activity in the thread's schedule
void register_activity(thread_t *thread, activity_t *activity, char *activity_name);

//! Monitor activities registered in the thread's schedule. Alive is not dead.
bool monitor_activities_alive(thread_t *thread);

#ifdef __cplusplus
} // extern "C"
#endif

#endif //THREAD_H

