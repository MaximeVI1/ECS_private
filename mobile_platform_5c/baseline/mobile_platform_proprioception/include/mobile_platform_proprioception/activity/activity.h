/* ----------------------------------------------------------------------------
 * Project Title,
 * ROB @ KU Leuven, Leuven, Belgium
 * See LICENSE for the license information
 * -------------------------------------------------------------------------- */

/**
 * @file mobile_platform_proprioception_activity.h
 * @date October 12, 2021

 **/

#ifndef MOBILE_PLATFORM_PROPRIOCEPTION_ACTIVITY_H
#define MOBILE_PLATFORM_PROPRIOCEPTION_ACTIVITY_H

#include <threads.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

// 5C
#include <five_c/activity/activity.h>

// Generic data structure includes
#include <navigation_data_structure/odometry2d.h>
#include <drive_data_structure/differential_drive.h>
#include <platform_data_structure/delta_tricycle.h>
#include <data_structure/time/timestamp.h>
// Control algorithms
#include <mobile_platform_proprioception/delta_tricycle.h>

// Configuration file
#include <read_file/read_file.h>

#ifdef __cplusplus
extern "C" {
#endif

// Parameters
typedef struct mobile_platform_proprioception_activity_params_s
{
    char configuration_file[200];
    differential_drive_t *rt_drive, drive;
    delta_tricycle_t *rt_platform, platform;
    odometry2d_t *rt_odometry;
    double sampling_time_tolerance; 
}mobile_platform_proprioception_activity_params_t;

// Continuous state
typedef struct mobile_platform_proprioception_activity_continuous_state_s{
    odometry2d_t odometry;
    timestamp_t previous_timestamp;
}mobile_platform_proprioception_activity_continuous_state_t;

// Discrete state
typedef struct mobile_platform_proprioception_activity_discrete_state_s{
}mobile_platform_proprioception_activity_discrete_state_t;

typedef struct proprioception_coordination_state_s {
    // Coordination flags
    bool *execution_request;
    bool *deleting_request;
    // Mutex
    mtx_t *drive_lock, *odometry_lock, *platform_lock;
} mobile_platform_proprioception_activity_coordination_state_t;

extern const activity_lcsm_t mobile_platform_proprioception_activity_lcsm;

int mobile_platform_proprioception_activity_load_configuration_file(
    char *path_to_file, activity_t *activity);

void mobile_platform_proprioception_activity_config(activity_t* activity);

#ifdef __cplusplus
}
#endif

#endif //MOBILE_PLATFORM_PROPRIOCEPTION_ACTIVITY_H
