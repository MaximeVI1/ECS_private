#include <five_c/thread/thread.h>

/**
 * Core functionality of this library. It runs the schedule that contains
 * the registered activities and sleep until the cycle time is satisfied.
 * The activities are executed in the order which they were registered.
 * When all the registered activities reach the dead state, the loop is 
 * interrupted.
 * @param[in] thread_casted_void five_c thread data structure casted to void*
 * */
void* do_thread_loop(void *thread_casted_void){
    thread_t *thread = (thread_t *) thread_casted_void;
    struct timespec start, end;
    uint64_t elapsed_time_us;
    bool activies_alive = true;

    while (activies_alive){
        clock_gettime(CLOCK_MONOTONIC_RAW, &start);
        thread->clocktime = start;
        execute_schedule(&thread->schedule);    
        activies_alive = monitor_activities_alive(thread);
        clock_gettime(CLOCK_MONOTONIC_RAW, &end);
        elapsed_time_us = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_nsec - start.tv_nsec) / 1000;
        // Sleep such that the whole loop thread->last cycle_time_us
        if ( thread->cycle_time_us > elapsed_time_us)
            usleep(thread->cycle_time_us - elapsed_time_us);
    }
}

/**
 * Function for properly initialise five_c thread data structure
 * @param[in] thread data structure to be initialised
 * @param[in] thread_name name of the thread
 * @param[in] cycle_time_ms thread cycle time in microseconds
 * */
void create_thread(thread_t *thread, char *thread_name, long int cycle_time_ms){
    thread->schedule.number_of_functions = 0;
    thread->nb_activities = 0;
    thread->cycle_time_us = (useconds_t) cycle_time_ms*1000; // miliseconds -> microseconds
}

/**
 * Register an activity in the thread's schedule, which means registering
 * the function do_activity(). The arguments is the data structure that 
 * corresponds to a particular activity.
 * @param[in] thread 
 * @param[in] activity activity to be registered
 * @param[in] activity_name name of the activity to be registered
 * */
void register_activity(thread_t *thread, activity_t *activity, char *activity_name){
    // Registering in the thread data structure the flag that indicates an activity is dead
    register_function(&thread->schedule, (function_ptr_t) do_activity, 
        activity, activity_name);
    activity->clocktime = &thread->clocktime;
    thread->activities_deletion_complete[thread->nb_activities] = &activity->state.lcsm_flags.deletion_complete;
    thread->nb_activities += 1;
}

/**
 * Monitor whether the registered activities are alive. If all activites
 * are in the "dead" LCSM state, it means that they are not alive. 
 * @param[in] thread five_c thread data stucture
 * @param[out] bool true if at least one activity is alive, false otherwise
 * */
bool monitor_activities_alive(thread_t *thread){
    bool activities_dead = true;
    for (int i=0; i<thread->nb_activities; i++)
        activities_dead &= *thread->activities_deletion_complete[i]; 
    return !activities_dead;
}
 
