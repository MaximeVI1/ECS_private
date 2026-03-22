/* ----------------------------------------------------------------------------
 * Wheel drives using 5C template
 * ROB @ KU Leuven, Leuven, Belgium
 * See LICENSE for the license information
 * -------------------------------------------------------------------------- */

/**
 * @file kelo_demo_command_line.c
 * @date May 25, 2022
 * @summary: The purpose of this file is to test the kelo wheel activity, which is 
 * the only activity that runs in this application. There are two independent threads
 * for printing the kelo sensor data and update the actuation commands
 * 
 * Correct usage:
 * <app-name> <control-mode> <left-wheel-setpoint> <right-wheel-setpoint>
 * 
 * control-mode: torque or velocity
 * setpoints are limited to MAX_KELO_CURRENT and MAX_KELO_ANGULAR_VELOCITY
 **/

#include <stdio.h>
#include <pthread.h>
#include <signal.h>
#include <string.h>
#include <math.h>
#include "sudden_jump_detection.h"
#include <stdlib.h>

// 5C
#include <five_c/thread/thread.h>
// 5C data structures
#include <data_structure/registration/hash_table_variable.h>
#include <mobile_platform_drive/activity/kelo_drive_activity.h>

#define KELO_CONFIGURATION_FILE "../../configuration/kelo.json"
#define KELO_THREAD_CYCLE_TIME_IN_MS 5
#define MAX_KELO_CURRENT 4.0
#define MAX_KELO_ANGULAR_VELOCITY 60.0
#define WINDOW_SIZE 500

enum STATUS_COMMAND_LINE_ARGUMENTS{INVALID_COMMAND_LINE_ARGUMENTS=-1,
  VALID_COMMAND_LINE_ARGUMENTS};

enum STATUS_COMMAND_LINE_ARGUMENTS arguments_from_command_line(int argc, char **argv);
void* print_sensor_data(void* activity);
void* set_actuation(void* activity);

// This function declaration is added for logging data into a file.
void* log_sensor_data(void* activity);

void* collision_detection(void* activity);


static void sigint_handler(int sig); 
void store_sensor_data(double time,double left_velocity,double right_velocity, SensorDataArrays *sensor_data_arrays );
uint8_t collision_detection_function(SensorDataArrays sensor_data, uint16_t win_len, float thr);
uint8_t sudden_jump_detection(sud_jump_cfg_t* config, sud_jump_res_t* result, double* data, uint16_t data_len);

bool deinitialisation_request = false;
bool configuration_request = false;
bool execution_request = true;
bool collision_detected = false;

uint16_t command_mode = 0;
double left_wheel_setpoint = 0;
double right_wheel_setpoint = 0;


void main(int argc, char** argv) {
  int counter_setup =0;
  signal(SIGINT, sigint_handler);

  if (arguments_from_command_line(argc, argv) == INVALID_COMMAND_LINE_ARGUMENTS){
    return;
  }

  // Table for variables
  variable_table_t variable_table;
  variable_table.root = NULL;
  mtx_init(&variable_table.mutex,mtx_plain);
  // Activity
  activity_t kelo_activity;
  mobile_platform_drive_kelo_activity_lcsm.create(&kelo_activity, "kelo_activity");
  mobile_platform_drive_kelo_activity_lcsm.configure(&kelo_activity, &variable_table);

  // Share memory
  mobile_platform_drive_kelo_activity_coordination_state_t *mobile_platform_drive_kelo_activity_coord_state =
      (mobile_platform_drive_kelo_activity_coordination_state_t *) kelo_activity.state.coordination_state;
  mobile_platform_drive_kelo_activity_params_t *mobile_platform_drive_kelo_activity_params =
      (mobile_platform_drive_kelo_activity_params_t *) kelo_activity.conf.params;
 
  mobile_platform_drive_kelo_activity_coord_state->deinitialisation_request = &deinitialisation_request;
  mobile_platform_drive_kelo_activity_coord_state->configuration_request = &configuration_request;
  mobile_platform_drive_kelo_activity_coord_state->execution_request = &execution_request;

  // Configuration files
  strcpy(mobile_platform_drive_kelo_activity_params->configuration_file, KELO_CONFIGURATION_FILE);

  // Threads
  thread_t thread_drive;
  create_thread(&thread_drive, "thread_drive", KELO_THREAD_CYCLE_TIME_IN_MS);
  register_activity(&thread_drive, &kelo_activity, "kelo_activity");
  
  // Run posix threads
  pthread_t pthread_drive, pthread_print, pthread_actuation, pthread_log, pthread_collision;
  pthread_create( &pthread_drive, NULL, do_thread_loop, ((void*) &thread_drive));
  pthread_create( &pthread_print, NULL, print_sensor_data, (void*) &kelo_activity); 
  pthread_create( &pthread_actuation, NULL, set_actuation, (void*) &kelo_activity); 
  pthread_create( &pthread_log, NULL, log_sensor_data, (void*) &kelo_activity); 
  pthread_create( &pthread_collision, NULL, collision_detection, (void*) &kelo_activity);
  
  // Wait pthread_drive to finish
  pthread_join( pthread_drive, NULL);
  pthread_join (pthread_log, NULL);
  pthread_join (pthread_collision, NULL);
  pthread_join (pthread_actuation, NULL);
  pthread_join (pthread_print, NULL);
}


enum STATUS_COMMAND_LINE_ARGUMENTS arguments_from_command_line(int argc, char **argv){
  if (argc!=1){
    printf("The correct usage is..\n");
    printf(".\\kelo_demo \n\n"); 
    return INVALID_COMMAND_LINE_ARGUMENTS;
  }

  // Command mode
  command_mode = DIFFERENTIAL_DRIVE_COMMAND_MODE_TORQUE;

  // Setpoint: starting torque +- = 1A 
  left_wheel_setpoint = -1.2;
  right_wheel_setpoint = 1.2;

  
 

  return VALID_COMMAND_LINE_ARGUMENTS;
}

/**
 * Prints to the console the sensor data from all the smart wheels connected to 
 * the master. The printing rate is 5 Hz
 * 
 * Note: kelo activity fills in the shared memory with real-time sensor data. 
 * This function copies to a local variable the data from the shared memory. 
 * Mutexes prevents data inconsistency (race conditions). After "unlocking"
 * the mutex, the function prints the sensor data  to the console.  
 */
void* print_sensor_data(void* activity){
  activity_t *kelo_activity = (activity_t*) activity; 

  mobile_platform_drive_kelo_activity_params_t *params =
      (mobile_platform_drive_kelo_activity_params_t *) kelo_activity->conf.params;
  mobile_platform_drive_kelo_activity_continuous_state_t *continuous_state =
    (mobile_platform_drive_kelo_activity_continuous_state_t *) kelo_activity->state.computational_state.continuous; 
  mobile_platform_drive_kelo_activity_coordination_state_t *coord_state =
    (mobile_platform_drive_kelo_activity_coordination_state_t *) kelo_activity->state.coordination_state;  

  differential_drive_sensor_t drive_sensor[MAX_NUMBER_SMARTWHEELS];
  
  while(!(deinitialisation_request)){
    if (kelo_activity->lcsm.state == RUNNING)
    {
      // Copying data
      for (int i=0; i<continuous_state->kelo_soem_state.nb_smart_wheel_detected; i++)
      {
        mtx_lock(coord_state->drive_lock[i]);
        drive_sensor[i] = params->rt_drive[i]->sensor;
        mtx_unlock(coord_state->drive_lock[i]);
      }
      if (!collision_detected){
        // Pr[i]inting
        for (int i=0; i<continuous_state->kelo_soem_state.nb_smart_wheel_detected; i++)
        {
          printf("------------------- Smart Wheel #%2.0d ----------------------\n", i+1);
          // Time
          double time = (double) drive_sensor[i].timestamp.seconds + drive_sensor[i].timestamp.nanoseconds*1e-9; 
          printf("time: %f secs\n", time);
          // Position
          printf("left wheel pos: %f rad, ", drive_sensor[i].encoder.left_wheel.position);
          printf("right wheel pos: %f rad, ", drive_sensor[i].encoder.right_wheel.position);
          printf("pivot pos: %f rad\n", drive_sensor[i].encoder.pivot.position);
          // Velocity
          printf("left wheel vel: %f rad/s, ", drive_sensor[i].encoder.left_wheel.velocity);
          printf("right wheel vel: %f rad/s, ", drive_sensor[i].encoder.right_wheel.velocity);
          printf("pivot vel: %f rad/s\n", drive_sensor[i].encoder.pivot.velocity);
        }
      }
    }
    usleep(200000); // time in microseconds
  }
}



// Thread for logging sensor data
// log file that is created is called kelo_log.txt

void* log_sensor_data(void* activity) {

    
    activity_t *kelo_activity = (activity_t*) activity; 

    mobile_platform_drive_kelo_activity_params_t *params =
        (mobile_platform_drive_kelo_activity_params_t *) kelo_activity->conf.params;
    mobile_platform_drive_kelo_activity_continuous_state_t *continuous_state =
        (mobile_platform_drive_kelo_activity_continuous_state_t *) kelo_activity->state.computational_state.continuous; 
    mobile_platform_drive_kelo_activity_coordination_state_t *coord_state =
        (mobile_platform_drive_kelo_activity_coordination_state_t *) kelo_activity->state.coordination_state;  

    differential_drive_sensor_t drive_sensor[MAX_NUMBER_SMARTWHEELS];
    
    // Open the log file for writing
    FILE *logfile_ptr;
    logfile_ptr = fopen("kelo_log.txt", "w");

    // Log the format declaration
    fprintf(logfile_ptr, "Time (secs)\tLeft Wheel Pos (rad)\tRight Wheel Pos (rad)\tPivot Pos (rad)\tLeft Wheel Vel (rad/s)\tRight Wheel Vel (rad/s)\tPivot Vel (rad/s)\tAccelerometer x ()\t Accelerometer y ()\tAccelerometer z ()\tGyroscope x ()\tGyroscope y ()\t Gyroscope z ()\tTemperature Right Wheel ()\tTemperature Left Wheel ()\tVoltage bus (V)\tCurrent Right wheel (A)\tCurrent Left Wheel (A)\n");
    printf("log file open\n");
    while (!(deinitialisation_request)) {
        if (kelo_activity->lcsm.state == RUNNING) 
        {
            // Copying data...
            for (int i=0; i<continuous_state->kelo_soem_state.nb_smart_wheel_detected; i++) 
            {
                mtx_lock(coord_state->drive_lock[i]);
                drive_sensor[i] = params->rt_drive[i]->sensor;
                mtx_unlock(coord_state->drive_lock[i]);
            }

            // Logging...
            for (int i=0; i<continuous_state->kelo_soem_state.nb_smart_wheel_detected; i++) 
            {
                // Time
                double time = (double) drive_sensor[i].timestamp.seconds + drive_sensor[i].timestamp.nanoseconds*1e-9; 
                // Log the values
                fprintf(logfile_ptr, "%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\n",
                    time,
                    drive_sensor[i].encoder.left_wheel.position,
                    drive_sensor[i].encoder.right_wheel.position,
                    drive_sensor[i].encoder.pivot.position,
                    drive_sensor[i].encoder.left_wheel.velocity,
                    drive_sensor[i].encoder.right_wheel.velocity,
                    drive_sensor[i].encoder.pivot.velocity,
                    drive_sensor[i].accelerometer.x,
                    drive_sensor[i].accelerometer.y,
                    drive_sensor[i].accelerometer.z,
                    drive_sensor[i].gyroscope.x,
                    drive_sensor[i].gyroscope.y,
                    drive_sensor[i].gyroscope.z,
                    drive_sensor[i].temperature.right_wheel,
                    drive_sensor[i].temperature.left_wheel, 
                    drive_sensor[i].voltage_bus,
                    drive_sensor[i].current_u.right_wheel,
                    drive_sensor[i].current_u.right_wheel);
                  
            }
        }
          
    }

    // Close the log file
    printf("log file closed\n");
    fclose(logfile_ptr);
}



/**
 *  Update the wheel's control mode and setpoints 
 */
void* set_actuation(void* activity){
  activity_t *kelo_activity = (activity_t*) activity; 
  mobile_platform_drive_kelo_activity_params_t* params = (mobile_platform_drive_kelo_activity_params_t *) kelo_activity->conf.params;
  mobile_platform_drive_kelo_activity_continuous_state_t *continuous_state =
    (mobile_platform_drive_kelo_activity_continuous_state_t *) kelo_activity->state.computational_state.continuous;
  mobile_platform_drive_kelo_activity_coordination_state_t *coord_state =
    (mobile_platform_drive_kelo_activity_coordination_state_t *) kelo_activity->state.coordination_state;  

  while(!(deinitialisation_request)){
    if (kelo_activity->lcsm.state == RUNNING)
    {
      for (int i=0; i<continuous_state->kelo_soem_state.nb_smart_wheel_detected; i++)
      {
        mtx_lock(coord_state->drive_lock[i]);
        if(params->rt_drive[i]->actuation.mode != command_mode)
        {
          params->rt_drive[i]->actuation.mode = command_mode;
          configuration_request = true;
        }
        switch(params->rt_drive[i]->actuation.mode)
        {
          case DIFFERENTIAL_DRIVE_COMMAND_MODE_VELOCITY:
            params->rt_drive[i]->actuation.velocity.left_wheel = left_wheel_setpoint;
            params->rt_drive[i]->actuation.velocity.right_wheel = right_wheel_setpoint;
            break;
          case DIFFERENTIAL_DRIVE_COMMAND_MODE_TORQUE:
            params->rt_drive[i]->actuation.current.left_wheel = left_wheel_setpoint;
            params->rt_drive[i]->actuation.current.right_wheel = right_wheel_setpoint;
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
  }

}

/**
 * Store wheel velocity's in array. 
 * Detect collision
 * Update wheel torque setpoint after collision
*/
void* collision_detection(void* activity){
    activity_t *kelo_activity = (activity_t*) activity; 

  mobile_platform_drive_kelo_activity_params_t *params =
      (mobile_platform_drive_kelo_activity_params_t *) kelo_activity->conf.params;
  mobile_platform_drive_kelo_activity_continuous_state_t *continuous_state =
    (mobile_platform_drive_kelo_activity_continuous_state_t *) kelo_activity->state.computational_state.continuous; 
  mobile_platform_drive_kelo_activity_coordination_state_t *coord_state =
    (mobile_platform_drive_kelo_activity_coordination_state_t *) kelo_activity->state.coordination_state;  

  differential_drive_sensor_t drive_sensor[MAX_NUMBER_SMARTWHEELS];
  
   printf("start collision detection\n");

    // Array's for wheel velocity's and sudden_jump_detection 'magic numbers'
     
    uint16_t win_len = 0.8 * 500;    // number of datapoints 
    float thr = 0.5;                 // Velocity(rad/s) threshold



  
    double time, left_velocity, right_velocity;


  
  
  while(!(deinitialisation_request) && !(collision_detected)){
    
    if (kelo_activity->lcsm.state == RUNNING)
    {
      
      // Copying data from mutex
      for (int i=0; i<continuous_state->kelo_soem_state.nb_smart_wheel_detected; i++)
      {
        mtx_lock(coord_state->drive_lock[i]);
        drive_sensor[i] = params->rt_drive[i]->sensor;
        mtx_unlock(coord_state->drive_lock[i]);
      }

      for (int i = 0; i < continuous_state->kelo_soem_state.nb_smart_wheel_detected; i++)
            {
            time = (double) drive_sensor[i].timestamp.seconds + drive_sensor[i].timestamp.nanoseconds*1e-9;
            left_velocity = drive_sensor[i].encoder.left_wheel.velocity; 
            right_velocity = drive_sensor[i].encoder.right_wheel.velocity;
            }
    
    SensorDataArrays sensor_data_arrays;
 


      // Store data in velocity array's 
      store_sensor_data(time,left_velocity,right_velocity, &sensor_data_arrays);
      

      // Check for sudden jump: zero if collision
      int collision_result = collision_detection_function(sensor_data_arrays, win_len, thr);

      if (collision_result == 0)
      {
        //update boolean: 
        collision_detected = true;
        printf("Collision detected. Mobile drive stopped. \n");

        //update torque setpoins
        left_wheel_setpoint = 0;
        right_wheel_setpoint = 0;
      }
    }
      
    }
}

/**
 * This function is called when ctrl+c is pressed.
 */
static void sigint_handler(int sig){
  printf("Deinitialising mobile platform drive activity\n");
  deinitialisation_request = true;
}

/**
 * Functions written by us that are used in the collision detection thread
*/

//Function 1

void store_sensor_data(double time,double left_velocity,double right_velocity, SensorDataArrays *sensor_data_arrays ) {   
       
    // Shift (FIFO behavior)
    for (int j = 0; j < WINDOW_SIZE-1; j++) {
        sensor_data_arrays->left_wheel_velocity[j] = sensor_data_arrays->left_wheel_velocity[j + 1];
        sensor_data_arrays->right_wheel_velocity[j] = sensor_data_arrays->right_wheel_velocity[j + 1];
        sensor_data_arrays->timestamp[j] = sensor_data_arrays->timestamp[j + 1];
    }

    sensor_data_arrays->left_wheel_velocity[WINDOW_SIZE-1] = left_velocity;
    sensor_data_arrays->right_wheel_velocity[WINDOW_SIZE-1] = right_velocity;
    sensor_data_arrays->timestamp[WINDOW_SIZE-1] = time;
    }
        
// Function 2

uint8_t collision_detection_function(SensorDataArrays sensor_data, uint16_t win_len, float thr) {
  
  sud_jump_cfg_t config;
  config.win_len = win_len;
  config.thr = thr;
  sud_jump_res_t result;

  int counter =0;

  // left wheel jump
  uint8_t detection_result_left = sudden_jump_detection(&config, &result,sensor_data.left_wheel_velocity, WINDOW_SIZE);
  if (detection_result_left == 1 && result.jump_found == 1 && result.jump_direction == 1) {
    counter++;    
  }
  

  // right wheel jump
  uint8_t detection_result_right = sudden_jump_detection(&config, &result,sensor_data.right_wheel_velocity, WINDOW_SIZE);
  if (detection_result_right == 1 && result.jump_found == 1 && result.jump_direction == 255) {
    counter++;
  }

  // 2 wheels jump detected
  if (counter > 1) {
    return 0; // Collision detected
    }
  return 1;
  }

// Function 3

uint8_t sudden_jump_detection(sud_jump_cfg_t* config, sud_jump_res_t* result, double* data, uint16_t data_len)
{
  result->jump_found=0;
  result->jump_direction=0;
  if (data_len < config->win_len)
  {
    return 0;
  }

  uint8_t win_uneven = 0;

  if (config->win_len % 2 == 1)
    { win_uneven = 1;}
  
  uint16_t sub_win_len = (config->win_len - win_uneven) / 2; 
  uint16_t index_window_left = 0;
  float average_left = 0;
  float average_right = 0;
  float difference = 0;
  uint16_t index_middle_win = 0;
 
  for (index_window_left = 0; index_window_left <= (data_len - config->win_len); index_window_left++)
  {
    /* calculate average of the two subwindows */
    average_left = 0;
    average_right = 0;

    for (uint16_t index_sub_window = 0; index_sub_window < sub_win_len; index_sub_window++)
    {
      
      average_left += data[index_window_left + index_sub_window];
      average_right += data[index_window_left + config->win_len - 1 - index_sub_window];
    }
    
    if (win_uneven == 1)
    {
      average_left += data[index_window_left + sub_win_len];
    }

    average_left /= (sub_win_len + win_uneven);
    average_right /= sub_win_len;

    /* calculate difference */
    difference = fabs(average_left - average_right);

    index_middle_win = index_window_left + sub_win_len - 1 + win_uneven;
    result->diff[index_middle_win] = difference;

    /* check for jump and jump direction*/ 
    if (difference > config->thr)
    {  
      result->jump_found = 1;
      

      if (average_left < average_right)
      {
          result->jump_direction = 1;
      }

      if (average_left > average_right)
      {
          result->jump_direction = -1;
      }
    }

        
  }

  return 1;

}
