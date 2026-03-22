/* ----------------------------------------------------------------------------
 * Wheel drives using 5C template
 * ROB @ KU Leuven, Leuven, Belgium
 * See LICENSE for the license information
 * -------------------------------------------------------------------------- */

/**
 * @file kelo_soem.h
 * @date October 13, 2021
 **/

#ifndef KELO_SOEM_H
#define KELO_SOEM_H

#define MAX_NUMBER_SMARTWHEELS 1

#include <stdio.h>
#include <stdbool.h>

//Kelo
#include <mobile_platform_drive/kelo/smart_wheel_api.h>

// Data structure
#include <drive_data_structure/differential_drive.h>

extern bool enabled_ecat_monitor_activity;

typedef struct kelo_soem_params_s{
    char ifname[20];
}kelo_soem_params_t;

typedef struct kelo_soem_state_s{
    rxpdo1_t *cmd_data_smart_wheel[MAX_NUMBER_SMARTWHEELS];
    txpdo1_t *sensor_data_smart_wheel[MAX_NUMBER_SMARTWHEELS]; 
    int sw_serial_number[MAX_NUMBER_SMARTWHEELS];
    char IOmap[4096]; // 4kB
    int expected_work_counter;
    bool has_new_measurement;

    int nb_smart_wheel_detected;
    boolean in_operational_state;
    uint32 smartWheelStatus[MAX_NUMBER_SMARTWHEELS];
    volatile int work_counter;

}kelo_soem_state_t;

/**
 * Detect smartwheels and initialise ethercat connection  
 * @param[inout] state continuous and discrete state of the connection
 * @param[in] params parameters of the connection
 */ 
bool initialise_ethercat_master(kelo_soem_state_t *state, kelo_soem_params_t *params);

/**
 * Shutdown ethercat connection  
 * @param[in] state continuous and discrete state of the connection
 * @param[in] params parameters of the connection
 */ 
void shutdown_ethercat_master(kelo_soem_state_t *state, kelo_soem_params_t *params);

/**
 * Read latest message from the smartwheel (see smart_wheel_api.h) and
 * write command and setpoint values  
 * @param[inout] state continuous and discrete state of the connection
 */ 
void read_write_ethercat(kelo_soem_state_t *state);

/**
 * Transform ethercat message sent by kelo wheel to aacal standard
 * smartwheel message  
 * @param[in] sensor_data_smart_wheel kelo wheel data structure as in smart_wheel_api.h
 * @param[out] drive_sensor differential drive sensor message defined in data-structure-5c
 */ 
void txpdo1_to_differential_drive_sensor(txpdo1_t *sensor_data_smart_wheel, 
    differential_drive_sensor_t *drive_sensor);

OSAL_THREAD_FUNC ecatcheck( void *ptr );
#endif //KELO_SOEM_H
