/* ----------------------------------------------------------------------------
 * Project Title,
 * ROB @ KU Leuven, Leuven, Belgium
 * See LICENSE for the license information
 * -------------------------------------------------------------------------- */

/**
 * @file navigation_activity.h
 * @date April 28, 2023
 **/

#ifndef NAVIGATION_ACTIVITY_H
#define NAVIGATION_ACTIVITY_H

#include <stdio.h>
#include <time.h>
#include <threads.h>

// 5C
#include <five_c/activity/activity.h>
// Data structure
#include <drive_data_structure/differential_drive.h>
#include <sensor_data_structure/range_sensor.h>
#include <sensor_data_structure/range_scan.h>
#include <navigation_data_structure/odometry2d.h>
#include <mechanics_data_structure/velocity.h>
#include <platform_data_structure/delta_tricycle.h>

// Logging
#include <mobile_platform_logging/tracepoint/logging-tp.h>

//! parameters
typedef struct navigation_activity_params_s{
    // Configuration file
    char configuration_file[250]; 
    // drive
    differential_drive_t *rt_drive, drive;
    // sensors
    range_sensor_t *rt_range_sensor, range_sensor;
    range_scan_t *rt_range_scan, range_scan;
    range_scan_float_t range_scan_float;
    // Perception
    odometry2d_t *rt_odometry, odometry;
    // Control
    velocity_t *rt_des_platform_velocity, des_platform_velocity;
    // Platform
    delta_tricycle_t *rt_platform, platform;
    // Logging counter
    int actuation_chunk_id;
}navigation_activity_params_t;

//! (computational) continuous state
typedef struct navigation_activity_continuous_state_s{
    // Add here (continuous) computational variables that 
    // you want to store during iterations of this activity.
}navigation_activity_continuous_state_t;

//! (computational) discrete state
typedef struct navigation_activity_discrete_state_s{
    // Add here (discrete) computational variables that you want to store
    // during iterations of this activity
}navigation_activity_discrete_state_t;

//! Coordination state
typedef struct navigation_activity_coordination_state_s {
    // Activity LCS
    bool *execution_request;
    bool *deleting_request;
    bool *configuration_request;
    // Mutex
    mtx_t *drive_lock, *range_scan_lock, *range_sensor_lock;
    mtx_t *odometry_lock, *des_platform_velocity_lock, *platform_lock;
} navigation_activity_coordination_state_t;

extern const activity_lcsm_t navigation_activity_lcsm;
#endif //MOBILE_PLATFORM_DRIVE_kelo_KELO_ACTIVITY_H

