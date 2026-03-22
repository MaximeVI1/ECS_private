/* ----------------------------------------------------------------------------
 * Wheel drives using 5C template
 * ROB @ KU Leuven, Leuven, Belgium
 * See LICENSE for the license information
 * -------------------------------------------------------------------------- */

/**
 * @file kelo_drive_activity.h
 * @date October 13, 2021
 **/

#ifndef MOBILE_PLATFORM_DRIVE_KELO_ACTIVITY_H
#define MOBILE_PLATFORM_DRIVE_KELO_ACTIVITY_H

#include <stdio.h>
#include <time.h>
#include <threads.h>

// ACCAL
#include <five_c/activity/activity.h>

//Kelo
#include <mobile_platform_drive/kelo/smart_wheel_api.h>
#include <mobile_platform_drive/kelo/kelo_soem.h>

#include <drive_data_structure/differential_drive.h>

#include <read_file/read_file.h>

// Logging
#include <mobile_platform_logging/tracepoint/logging-tp.h>

//! parameters
typedef struct mobile_platform_drive_kelo_activity_params_s{
    char configuration_file[100];
    int max_number_of_connection_attempts;
    float min_time_between_connection_attempts;
    kelo_soem_params_t kelo_soem_params;
    differential_drive_t *rt_drive[MAX_NUMBER_SMARTWHEELS], drive[MAX_NUMBER_SMARTWHEELS];
    struct{
        char name[50];
        int sw_serial_number;
    }id[MAX_NUMBER_SMARTWHEELS];
    int sensor_chunk_id;
}mobile_platform_drive_kelo_activity_params_t;

//! (computational) continuous state
typedef struct mobile_platform_drive_kelo_activity_continuous_state_s{
    struct timespec clocktime_at_last_connection_attempt;
    kelo_soem_state_t kelo_soem_state;
}mobile_platform_drive_kelo_activity_continuous_state_t;

//! (computational) discrete state
typedef struct mobile_platform_drive_kelo_activity_discrete_state_s{
    bool ethercat_master_connected;
    int number_of_connection_attempts;
}mobile_platform_drive_kelo_activity_discrete_state_t;

//! Coordination state
typedef struct mobile_platform_drive_kelo_activity_coordination_state_s {
    // Activity LCS
    bool *execution_request;
    bool *deleting_request;
    bool *configuration_request;
    // Operation mode
    bool disable_wheels_request;
    bool enable_wheels_request;
    // Mutex
    mtx_t *drive_lock[MAX_NUMBER_SMARTWHEELS];
} mobile_platform_drive_kelo_activity_coordination_state_t;

void configure_mobile_platform_drive_kelo_activity_from_file(const char *file_path,
    mobile_platform_drive_kelo_activity_params_t *params, int *status);

extern const activity_lcsm_t mobile_platform_drive_kelo_activity_lcsm;
#endif //MOBILE_PLATFORM_DRIVE_kelo_KELO_ACTIVITY_H

