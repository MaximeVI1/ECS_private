/* ----------------------------------------------------------------------------
 * Project Title,
 * ROB @ KU Leuven, Leuven, Belgium
 * See LICENSE for the license information
 * -------------------------------------------------------------------------- */

/**
 * @file logging_demo.c
 * @date may 15, 2025
 * @summary: The purpose of this file is to test the communication activity, 
 * which should run together with a navigation base device. 
 * 
 * Correct usage:
 **/
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>
#include <threads.h>
#include <math.h>

#include <mobile_platform_logging/activity/activity.h>
#include <five_c/thread/thread.h>

#include <data_structure/registration/hash_table_variable.h>
#include <sensor_data_structure/encoder.h>
#include <sensor_data_structure/range_scan.h>

#define NUMBER_OF_RANGE_BEAMS 5
#define LIDAR_FREQUENCY_IN_HZ 10
#define ENCODER_FREQUENCY_IN_HZ 100

static void sigint_handler(int sig); 
int mock_lidar_activity(void *args);
int mock_encoder_activity(void *args);
int mock_communication_activity(void *args);
void print_lidar(range_scan_t *range_scan, mtx_t *mutex);
void print_encoder(encoder_t *encoder, mtx_t *mutex);

static void sigint_handler(int sig); 
int arguments_from_command_line(int argc, char **argv);

bool deinitialisation_request = false;
bool execution_request = true;

typedef struct {
    char dataname[50];
    char datatype[50];
    variable_table_t *table;
}mock_activity_t;


int main(int argc, char** argv) {
  signal(SIGINT, sigint_handler);

  // if (arguments_from_command_line(argc, argv) == INVALID_COMMAND_LINE_ARGUMENTS){
  //   return;
  // }

  variable_table_t table;
  create_variable_table(&table);

  activity_t mobile_platform_logging_activity;
  mobile_platform_logging_activity_lcsm.create(&mobile_platform_logging_activity, "mobile_platform_logging_activity");
  mobile_platform_logging_activity_lcsm.configure(&mobile_platform_logging_activity,&table);

    // Share memory
  mobile_platform_logging_activity_params_t *mobile_platform_logging_activity_params =
      (mobile_platform_logging_activity_params_t *) mobile_platform_logging_activity.conf.params;
  mobile_platform_logging_activity_coordination_state_t *mobile_platform_logging_activity_coord_state =
      (mobile_platform_logging_activity_coordination_state_t *) mobile_platform_logging_activity.state.coordination_state;

  mobile_platform_logging_activity_coord_state->execution_request = &execution_request;

    // Creating threads
  thrd_t communication_thrd;

  thread_t thread_comm;
  create_thread(&thread_comm, "thread_comm", 500); // time in ms
  register_activity(&thread_comm, &mobile_platform_logging_activity, "mobile_platform_logging_activity");
  pthread_t pthread_comm;
  pthread_create( &pthread_comm, NULL, do_thread_loop, ((void*) &thread_comm));

  int res;
  thrd_join(communication_thrd, &res);
  pthread_join( pthread_comm, NULL);
  // mobile_platform_logging_activity_lcsm.destroy(&mobile_platform_logging_activity);
  return 0;

}


/**
 * This function is called when ctrl+c is pressed.
 */
static void sigint_handler(int sig){
  printf("Deinitialising communication activity\n");
  deinitialisation_request = true;
}

