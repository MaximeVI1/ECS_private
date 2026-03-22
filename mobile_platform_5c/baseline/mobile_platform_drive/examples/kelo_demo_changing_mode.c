/* ----------------------------------------------------------------------------
 * Project Title,
 * ROB @ KU Leuven, Leuven, Belgium
 * See LICENSE for the license information
 * -------------------------------------------------------------------------- */

/**
 * @file kelo_demo_changing_mode.c
 * @date May 25, 2022
 * @summary: The purpose of this file is to test the kelo wheel activity, which is 
 * the only activity that runs in this application. There are two independent threads
 * for printing the kelo sensor data and update the actuation commands
 * 
 * Correct usage:
 * <app-name> 
 * 
 **/

#include <stdio.h>
#include <pthread.h>
#include <signal.h>
#include <string.h>
#include <math.h>

// 5C
#include <five_c/activity/activity.h>
#include <five_c/thread/thread.h>
// 5C data structure
#include <data_structure/registration/hash_table_variable.h>
// Drive activity
#include <mobile_platform_drive/activity/kelo_drive_activity.h>

#define KELO_CONFIGURATION_FILE "../examples/configuration/kelo.json"
#define KELO_THREAD_CYCLE_TIME_IN_MSEC 5
#define CHANGING_MODE_PERIOD_IN_SEC 5
#define WHEEL_VELOCITY_SETPOINT 40.0
#define WHEEL_CURRENT_SETPOINT 0.35

void* set_actuation(void* activity);
static void sigint_handler(int sig); 

bool deinitialisation_request = false;
bool configuration_request = false;
bool execution_request = true;

void main(int argc, char** argv) {
  signal(SIGINT, sigint_handler);

  // Table for variables
  variable_table_t variable_table;

  // Activity
  activity_t kelo_drive_activity;
  mobile_platform_drive_kelo_activity_lcsm.create(&kelo_drive_activity, "kelo_activity");
  mobile_platform_drive_kelo_activity_lcsm.configure(&kelo_drive_activity, &variable_table);

  // Share memory
  mobile_platform_drive_kelo_activity_coordination_state_t *kelo_drive_activity_coord_state =
      (mobile_platform_drive_kelo_activity_coordination_state_t *) kelo_drive_activity.state.coordination_state;
  mobile_platform_drive_kelo_activity_params_t *kelo_drive_activity_params =
      (mobile_platform_drive_kelo_activity_params_t *) kelo_drive_activity.conf.params;
 
  kelo_drive_activity_coord_state->deinitialisation_request = &deinitialisation_request;
  kelo_drive_activity_coord_state->configuration_request = &configuration_request;
  kelo_drive_activity_coord_state->execution_request = &execution_request;

  // Configuration files
  strcpy(kelo_drive_activity_params->configuration_file, KELO_CONFIGURATION_FILE);

  // Threads
  thread_t thread_drive;
  create_thread(&thread_drive, "thread_drive", KELO_THREAD_CYCLE_TIME_IN_MSEC);
  register_activity(&thread_drive, &kelo_drive_activity, "kelo_drive_activity");
  // Run posix threads
  pthread_t pthread_drive, pthread_print, pthread_actuation;
  pthread_create( &pthread_drive, NULL, do_thread_loop, ((void*) &thread_drive));
  pthread_create( &pthread_actuation, NULL, set_actuation, (void*) &kelo_drive_activity); 
  // Wait pthread_drive to finish 
  pthread_join( pthread_drive, NULL);
}

/**
 *  Update the wheel's control mode and setpoints 
 */
void* set_actuation(void* activity){
  activity_t *kelo_drive_activity = (activity_t*) activity; 
  mobile_platform_drive_kelo_activity_params_t* params = (mobile_platform_drive_kelo_activity_params_t *) kelo_drive_activity->conf.params;
  mobile_platform_drive_kelo_activity_continuous_state_t *continuous_state =
    (mobile_platform_drive_kelo_activity_continuous_state_t *) kelo_drive_activity->state.computational_state.continuous;
  mobile_platform_drive_kelo_activity_coordination_state_t *coord_state =
    (mobile_platform_drive_kelo_activity_coordination_state_t *) kelo_drive_activity->state.coordination_state;  

  double time_since_last_mode_change = 0;
  uint16_t command_mode = 0;
  double left_wheel_setpoint = 0;
  double right_wheel_setpoint = 0;

  while(!(deinitialisation_request)){
    if (kelo_drive_activity->lcsm.state == RUNNING){
      // Copying data
      if (time_since_last_mode_change > CHANGING_MODE_PERIOD_IN_SEC){
        printf("changing mode to.. ");
        if (command_mode == DIFFERENTIAL_DRIVE_COMMAND_MODE_VELOCITY){
          printf("TORQUE!\n");
          command_mode = DIFFERENTIAL_DRIVE_COMMAND_MODE_TORQUE;
        }else{
          printf("VELOCITY!\n");
          command_mode = DIFFERENTIAL_DRIVE_COMMAND_MODE_VELOCITY;
        }
        time_since_last_mode_change = 0;
      }
      for (int i=0; i<continuous_state->kelo_soem_state.nb_smart_wheel_detected; i++){
        mtx_lock(coord_state->drive_lock[i]);
        // Check if current command_mode (rt_drive) is the same as the desired command_mode
        if(params->rt_drive[i]->actuation.mode != command_mode){
          params->rt_drive[i]->actuation.mode = command_mode;
          configuration_request = true;
        }
        switch(params->rt_drive[i]->actuation.mode){
          case DIFFERENTIAL_DRIVE_COMMAND_MODE_VELOCITY:
            params->rt_drive[i]->actuation.velocity.left_wheel = WHEEL_VELOCITY_SETPOINT;
            params->rt_drive[i]->actuation.velocity.right_wheel = WHEEL_VELOCITY_SETPOINT;
            break;
          case DIFFERENTIAL_DRIVE_COMMAND_MODE_TORQUE:
            params->rt_drive[i]->actuation.current.left_wheel = WHEEL_CURRENT_SETPOINT;
            params->rt_drive[i]->actuation.current.right_wheel = WHEEL_CURRENT_SETPOINT;
            break;
          default:
            params->rt_drive[i]->actuation.velocity.left_wheel = 0;
            params->rt_drive[i]->actuation.velocity.right_wheel = 0;
            params->rt_drive[i]->actuation.current.left_wheel = 0;
            params->rt_drive[i]->actuation.current.right_wheel = 0;
            break;
        }
        mtx_unlock(coord_state->drive_lock[i]);
      }

    }
    sleep(1);  // time in seconds
    time_since_last_mode_change += 1;
  }
}

/**
 * This function is called when ctrl+c is pressed.
 */
static void sigint_handler(int sig){
  printf("Deinitialising mobile platform drive activity\n");
  deinitialisation_request = true;
}
