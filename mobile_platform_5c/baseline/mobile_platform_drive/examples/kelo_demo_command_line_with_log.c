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

// 5C
#include <five_c/thread/thread.h>
// 5C data structures
#include <data_structure/registration/hash_table_variable.h>
#include <mobile_platform_drive/activity/kelo_drive_activity.h>

#define KELO_CONFIGURATION_FILE "../../configuration/kelo.json"
#define KELO_THREAD_CYCLE_TIME_IN_MS 5
#define MAX_KELO_CURRENT 4.0
#define MAX_KELO_ANGULAR_VELOCITY 60.0

enum STATUS_COMMAND_LINE_ARGUMENTS{INVALID_COMMAND_LINE_ARGUMENTS=-1,
  VALID_COMMAND_LINE_ARGUMENTS};

enum STATUS_COMMAND_LINE_ARGUMENTS arguments_from_command_line(int argc, char **argv);
void* print_sensor_data(void* activity);
void* set_actuation(void* activity);
static void sigint_handler(int sig); 

bool deinitialisation_request = false;
bool configuration_request = false;
bool execution_request = true;

uint16_t command_mode = 0;
double left_wheel_setpoint = 0;
double right_wheel_setpoint = 0;

// This function declaration is added for logging data into a file.
void* log_sensor_data(void* activity);

void main(int argc, char** argv) {
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
  pthread_t pthread_drive, pthread_print, pthread_actuation, pthread_log;
  pthread_create( &pthread_drive, NULL, do_thread_loop, ((void*) &thread_drive));
  pthread_create( &pthread_print, NULL, print_sensor_data, (void*) &kelo_activity); 
  pthread_create( &pthread_actuation, NULL, set_actuation, (void*) &kelo_activity); 
  pthread_create( &pthread_log, NULL, log_sensor_data, (void*) &kelo_activity); 
  
  // Wait pthread_drive to finish
  pthread_join( pthread_drive, NULL);
  pthread_join (pthread_log, NULL);
  pthread_join (pthread_actuation, NULL);
  pthread_join (pthread_print, NULL);
}

/**
 * Read arguments from command line.
 * <app-name> <control-mode> <left-wheel-setpoint> <right-wheel-setpoint>
 * 
 * control-mode: torque or velocity
 * setpoints: must be within [-2,2]
 */
enum STATUS_COMMAND_LINE_ARGUMENTS arguments_from_command_line(int argc, char **argv){
  if (argc!=4){
    printf("The correct usage is..\n");
    printf(".\\kelo_demo <actuation mode> <left_wheel_setpoint> <right_wheel_setpoint> \n\n"); 
    printf("- actuation mode can be either 'torque' or 'velocity'\n");
    printf("- the values of the current (torque mode) must be within [-%f,%f] A\n", 
      MAX_KELO_CURRENT, MAX_KELO_CURRENT);
    printf("- the values of the wheel velocity must be within [-%f,%f] rad/s\n\n", 
      MAX_KELO_ANGULAR_VELOCITY, MAX_KELO_ANGULAR_VELOCITY);
    return INVALID_COMMAND_LINE_ARGUMENTS;
  }

  // Check command mode 
  if (strcmp(argv[1], "torque") == 0){
    command_mode = DIFFERENTIAL_DRIVE_COMMAND_MODE_TORQUE;
  }else if (strcmp(argv[1], "velocity") == 0){
    command_mode = DIFFERENTIAL_DRIVE_COMMAND_MODE_VELOCITY;
  }else{
    printf("Unknown actuation mode.");
    printf("The valid option are 'torque' and 'velocity'\n\n");
    return INVALID_COMMAND_LINE_ARGUMENTS;
  }

  // Setpoint 
  left_wheel_setpoint = atof(argv[2]);
  right_wheel_setpoint = atof(argv[3]);

  if (command_mode == DIFFERENTIAL_DRIVE_COMMAND_MODE_TORQUE){
    if ( fabs(left_wheel_setpoint) > MAX_KELO_CURRENT || fabs(right_wheel_setpoint) > MAX_KELO_CURRENT){
      printf("ATTENTION: Setpoint is too high!\n");
      printf("The values of the torque (current) setpoints must be within [-%f,%f] A\n\n", 
        MAX_KELO_CURRENT, MAX_KELO_CURRENT);
      return INVALID_COMMAND_LINE_ARGUMENTS;
    }
  }else{
    if ( fabs(left_wheel_setpoint) > MAX_KELO_ANGULAR_VELOCITY || fabs(right_wheel_setpoint) > MAX_KELO_ANGULAR_VELOCITY){
      printf("ATTENTION: Setpoint is too high!\n");
      printf("The values of the wheel velocity setpoints must be within [-%f,%f] rad/s\n\n", 
        MAX_KELO_ANGULAR_VELOCITY, MAX_KELO_ANGULAR_VELOCITY);
      return INVALID_COMMAND_LINE_ARGUMENTS;
    }
  }

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
    usleep(200000); // time in microseconds
  }
}



// Function for logging sensor data
// log file that is created is called kelo_log.txt
// the parameters that are added onto this log file are (t, lw_pos, rw_pos, piv_pos, lw_vel, rw_vel, piv_vel)

// parameters that still can and should be added 

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
    printf("log file open");
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
                    drive_sensor[i].current_u.left_wheel);
                  
            }
        }
        usleep(2000); // time in microseconds    
    }

    // Close the log file
    printf("log file closed");
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
 * This function is called when ctrl+c is pressed.
 */
static void sigint_handler(int sig){
  printf("Deinitialising mobile platform drive activity\n");
  deinitialisation_request = true;
}
