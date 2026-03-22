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
  mock_activity_t lidar_activity = {.dataname="hokuyo", .table=&table};    // datatype: range_scan_t
  mock_activity_t left_encoder_activity = {.dataname="left", .table=&table};  // datatype: encoder_t
  mock_activity_t right_encoder_activity = {.dataname="right", .table=&table};

  activity_t comm_activity;
  comm_activity_lcsm.create(&comm_activity, "logging_activity");
  comm_activity_lcsm.configure(&comm_activity,&table);

    // Share memory
  comm_activity_params_t *comm_activity_params =
      (comm_activity_params_t *) comm_activity.conf.params;
  comm_activity_coordination_state_t *comm_activity_coord_state =
      (comm_activity_coordination_state_t *) comm_activity.state.coordination_state;

  comm_activity_coord_state->execution_request = &execution_request;
  comm_activity_coord_state->deinitialisation_request = &deinitialisation_request;
  
  // configuration
  strcpy(comm_activity_params->configuration_file, argv[1]);

      
    // Creating threads
  thrd_t lidar_thrd, left_encoder_thrd, right_encoder_thrd;
  thrd_t communication_thrd;
  thrd_create(&lidar_thrd, mock_lidar_activity, &lidar_activity);
  thrd_create(&left_encoder_thrd, mock_encoder_activity, &left_encoder_activity);
  thrd_create(&right_encoder_thrd, mock_encoder_activity, &right_encoder_activity);
  
  thread_t thread_comm;
  create_thread(&thread_comm, "thread_comm", 500); // time in ms
  register_activity(&thread_comm, &comm_activity, "comm_activity");
  pthread_t pthread_comm;
  pthread_create( &pthread_comm, NULL, do_thread_loop, ((void*) &thread_comm));

  int res;
  thrd_join(lidar_thrd, &res);
  thrd_join(left_encoder_thrd, &res);
  thrd_join(right_encoder_thrd, &res);
  thrd_join(communication_thrd, &res);
  pthread_join( pthread_comm, NULL);
  comm_activity_lcsm.destroy(&comm_activity);
  return 0;

}

void print_lidar(range_scan_t *range_scan, mtx_t *mutex)
{
    printf("range_scan: ");
    mtx_lock(mutex);
    for(int i=0; i<range_scan->nb_measurements; i++)
    {
        printf("%.3f ", range_scan->measurements[i]);
    }
    mtx_unlock(mutex);
    printf("\n");
}

void print_encoder(encoder_t *encoder, mtx_t *mutex)
{
    mtx_lock(mutex);
    printf("encoder pulses: %d\n", encoder->pulses);
    printf("encoder pos: %f\n", encoder->position);
    printf("encoder vel: %f\n", encoder->velocity);
    mtx_unlock(mutex);
}

int mock_lidar_activity(void *args)
{
    mock_activity_t *activity = (mock_activity_t *) args;
    variable_table_t *table = activity->table;
    hash_result_t hash_result;
    char hash_result_string[30];

    // Registration
    semantic_id_t data_sid;
    memset(&data_sid, 0, sizeof(data_sid));
    strcpy(data_sid.model, activity->dataname);
    variable_registration_args_t encoder_reg_args = {.data=data_sid};
    REGISTER_VARIABLE_IN_TABLE(table, encoder_reg_args, range_scan_t, hash_result);
    hash_result_to_string(hash_result, hash_result_string);
    printf("[mock_lidar_activity][REGISTER] %s range_scan_t: %s \n", 
       activity->dataname, hash_result_string);

    // Get pointer
    range_scan_t *range_scan = NULL;
    mtx_t *mutex = NULL;
    GET_VARIABLE_IN_TABLE(table, encoder_reg_args, range_scan_t, range_scan, mutex, hash_result);
    hash_result_to_string(hash_result, hash_result_string);
    printf("[mock_lidar_activity][GET] %s range_scan_t: %s at void pointer %p, lock with %p\n", 
       activity->dataname, hash_result_string, range_scan, mutex);
    // Check if succeded
    if(hash_result == HASH_ENTRY_NOT_FOUND)
    {
        return 0;
    }
    
    // // // Memory allocation for range scan (resource configure)
    range_scan->nb_measurements = NUMBER_OF_RANGE_BEAMS;
    range_scan->measurements = (double *) malloc(sizeof(double)*NUMBER_OF_RANGE_BEAMS);
    range_scan->angles = (double *) malloc(sizeof(double)*NUMBER_OF_RANGE_BEAMS);

    // Faking lidar data (compute)
    int sleeping_time = (int) ( (1.0/LIDAR_FREQUENCY_IN_HZ) *1000*1000);
    while(!deinitialisation_request)
    {
        usleep(sleeping_time); // time in microseconds
        mtx_lock(mutex);
        for(int i=0; i<range_scan->nb_measurements; i++)
        {
            range_scan->measurements[i] = ((double) rand())/RAND_MAX;
            range_scan->angles[i] = (M_PI/2*i)/(NUMBER_OF_RANGE_BEAMS-1) - M_PI/2 ;
        }
        clock_gettime(CLOCK_MONOTONIC_RAW, &range_scan->timestamp);
        mtx_unlock(mutex);
    }
}

int mock_encoder_activity(void *args)
{
    mock_activity_t *activity = (mock_activity_t *) args;
    variable_table_t *table = activity->table;
    hash_result_t hash_result;
    char hash_result_string[30];

    // Register and get variable
    semantic_id_t data_sid;
    strcpy(data_sid.model, activity->dataname); 
    variable_registration_args_t encoder_reg_args = {.data=data_sid};

    encoder_t *encoder = NULL;
    mtx_t *mutex = NULL;
    REGISTER_AND_GET_VARIABLE_IN_TABLE(table, encoder_reg_args, encoder_t, encoder, mutex, hash_result);
    printf("[mock_lidar_activity][REGISTER/GET] %s encoder_t: %s at void pointer %p, lock with %p\n", 
       activity->dataname, hash_result_string, encoder, mutex);
    
    // Check if succeded
    if(hash_result != HASH_ENTRY_ADDED)
    {
        return 0;
    }
    
    // Faking encoder data (compute)
    int sleeping_time = (int) ( (1.0/ENCODER_FREQUENCY_IN_HZ) *1000*1000);
    while(!deinitialisation_request)
    {
        usleep(sleeping_time); // time in microseconds
        mtx_lock(mutex);
        encoder->velocity = .1;
        encoder->position += encoder->velocity/ENCODER_FREQUENCY_IN_HZ;
        encoder->pulses += 1;
        clock_gettime(CLOCK_MONOTONIC_RAW, &encoder->timestamp);
        mtx_unlock(mutex);
    }
}

/**
 * Read arguments from command line.
 */
// int arguments_from_command_line(int argc, char **argv){
//   if (argc != 2){
//     printf("Incorrect usage! Please provide a configuration file, e.g.\n");
//     printf(".<executable> path_to_configuration_file.json\n\n");
//     return INVALID_COMMAND_LINE_ARGUMENTS;
//   }else{
//     return VALID_COMMAND_LINE_ARGUMENTS;
//   }
// }

/**
 * This function is called when ctrl+c is pressed.
 */
static void sigint_handler(int sig){
  printf("Deinitialising communication activity\n");
  deinitialisation_request = true;
}

