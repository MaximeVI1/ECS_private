/* ----------------------------------------------------------------------------
 * Project Title,
 * ROB @ KU Leuven, Leuven, Belgium
 * See LICENSE for the license information
 * -------------------------------------------------------------------------- */

/**
 * @file activity.c
 * @date Nov 19, 2021
 **/

#include <math.h>
#include <string.h>
#include <mobile_platform_proprioception/activity/activity.h>
#include <data_structure/registration/hash_table_variable.h>

/** 
 * The config() has to be scheduled everytime a change in the LCSM occurs, 
 * so it properly configures the schedule for the next iteration according
 * to the LCSM state, resources, task, ..  
 * @param[in] activity data structure for the command line activity
*/
void mobile_platform_proprioception_activity_config(activity_t* activity)
{
    // Remove config() from the eventloop schedule in the next iteration
    remove_schedule_from_eventloop(&activity->schedule_table, "activity_config");
    // Deciding which schedule to add
    switch (activity->lcsm.state)
    {
        case RESOURCE_CONFIGURATION:
            add_schedule_to_eventloop(&activity->schedule_table, "resource_configuration");
            break;
        case PAUSING:
            add_schedule_to_eventloop(&activity->schedule_table, "pausing");
            break;
        case RUNNING:
            add_schedule_to_eventloop(&activity->schedule_table, "running");
            break;
    }
};

// Creation
void mobile_platform_proprioception_activity_creation_coordinate(activity_t *activity)
{
    if (activity->state.lcsm_flags.creation_complete)
    {
        switch (activity->state.lcsm_protocol)
        { 
            case INITIALISATION:
                activity->lcsm.state = PAUSING;
                break;
            case EXECUTION:
                activity->lcsm.state = RUNNING;
                break;
            case DEINITIALISATION:
                activity->lcsm.state = DONE;
                activity->state.lcsm_flags.deletion_complete = true;
                break;
        }
        update_super_state_lcsm_flags(&activity->state.lcsm_flags, activity->lcsm.state);
    }
}

void mobile_platform_proprioception_activity_creation_configure(activity_t *activity)
{
    if (activity->lcsm.state != CREATION)
    {
        // Update schedule
        add_schedule_to_eventloop(&activity->schedule_table, "activity_config");
        remove_schedule_from_eventloop(&activity->schedule_table, "creation");
        // Update flags for next visit to the resource configuration LCS 
        activity->state.lcsm_flags.creation_complete = false;
    }
}

void mobile_platform_proprioception_activity_creation_compute(activity_t *activity)
{
    mobile_platform_proprioception_activity_continuous_state_t *continuous_state = 
        (mobile_platform_proprioception_activity_continuous_state_t *) activity->state.computational_state.continuous;
    mobile_platform_proprioception_activity_params_t *params = 
        (mobile_platform_proprioception_activity_params_t *) activity->conf.params;
    mobile_platform_proprioception_activity_coordination_state_t *coord_state = 
        (mobile_platform_proprioception_activity_coordination_state_t *) activity->state.coordination_state;
   
    // Wait until memory has been allocated for the following pointers:
    if (coord_state->deleting_request != NULL &&
        coord_state->execution_request != NULL)
    {
        // Initialise values
        memset(&continuous_state->odometry, 0, sizeof(odometry2d_t));   
        activity->state.lcsm_flags.creation_complete = true;
    }
}

void mobile_platform_proprioception_activity_creation(activity_t *activity)
{
    mobile_platform_proprioception_activity_creation_coordinate(activity);
    mobile_platform_proprioception_activity_creation_configure(activity);
    mobile_platform_proprioception_activity_creation_compute(activity);
}

// Resource configuration
void mobile_platform_proprioception_activity_resource_configuration_coordinate(activity_t *activity)
{
    mobile_platform_proprioception_activity_coordination_state_t *coord_state = 
        (mobile_platform_proprioception_activity_coordination_state_t *) activity->state.coordination_state;
        
    if (*coord_state->deleting_request)
    {
        activity->state.lcsm_protocol = DEINITIALISATION;
        activity->state.lcsm_flags.resource_configuration_complete = true;
    }
        
    if (activity->state.lcsm_flags.resource_configuration_complete)
    {
        switch (activity->state.lcsm_protocol)
        { 
            case INITIALISATION:
                activity->lcsm.state = PAUSING;
                break;
            case EXECUTION:
                activity->lcsm.state = RUNNING;
                break;
            case DEINITIALISATION:
                activity->lcsm.state = DONE;
                activity->state.lcsm_flags.deletion_complete = true;
                break;
        }
        update_super_state_lcsm_flags(&activity->state.lcsm_flags, activity->lcsm.state);
    }
}

void mobile_platform_proprioception_activity_resource_configuration_configure(activity_t *activity)
{
    if (activity->lcsm.state != RESOURCE_CONFIGURATION){
        // Update schedule
        add_schedule_to_eventloop(&activity->schedule_table, "activity_config");
        remove_schedule_from_eventloop(&activity->schedule_table, "resource_configuration");
        // Update flags for next visit to the resource configuration LCS 
        activity->state.lcsm_flags.resource_configuration_complete = false;
    }
}

void mobile_platform_proprioception_activity_resource_configuration_compute(activity_t *activity)
{
    mobile_platform_proprioception_activity_continuous_state_t *continuous_state = 
        (mobile_platform_proprioception_activity_continuous_state_t *) activity->state.computational_state.continuous;
    mobile_platform_proprioception_activity_params_t *params = 
        (mobile_platform_proprioception_activity_params_t *) activity->conf.params;
    mobile_platform_proprioception_activity_coordination_state_t *coord_state = 
        (mobile_platform_proprioception_activity_coordination_state_t *) activity->state.coordination_state;
   
    // Initialise values
    if(mobile_platform_proprioception_activity_load_configuration_file(params->configuration_file,
        activity) == CONFIGURATION_FROM_FILE_SUCCEEDED)
    {
        activity->state.lcsm_flags.resource_configuration_complete = true;
        if (params->rt_drive == NULL)
        {
            hash_result_t hash_result;
            variable_registration_args_t drive_registration_args = {
                .data ={.model= "kelo_drive"}
            };
            GET_VARIABLE_IN_TABLE(activity->table, drive_registration_args, 
                differential_drive_t, params->rt_drive, coord_state->drive_lock, 
                hash_result);
            if(hash_result == HASH_ENTRY_NOT_FOUND)
                activity->state.lcsm_flags.resource_configuration_complete = false;
        } 
        if (params->rt_platform == NULL)
        {
            hash_result_t hash_result;
            variable_registration_args_t platform_registration_args = {
                .data ={.model= "platform"}
            };
            REGISTER_AND_GET_VARIABLE_IN_TABLE(activity->table, platform_registration_args, 
                delta_tricycle_t, params->rt_platform, coord_state->platform_lock, hash_result);
            if(hash_result!=HASH_ENTRY_ADDED)
                activity->state.lcsm_flags.resource_configuration_complete = false;
            else
                memcpy(params->rt_platform, &params->platform, sizeof(delta_tricycle_t));
        } 
        if  (params->rt_odometry == NULL)
        {
            hash_result_t hash_result;
            variable_registration_args_t odometry_registration_args = {
                .data ={.model= "odometry"}
            };
            REGISTER_AND_GET_VARIABLE_IN_TABLE(activity->table, odometry_registration_args, 
                odometry2d_t, params->rt_odometry, coord_state->odometry_lock, hash_result);
            if(hash_result!=HASH_ENTRY_ADDED)
                activity->state.lcsm_flags.resource_configuration_complete = false;
            else
            {
                memset(params->rt_odometry, 0, sizeof(odometry2d_t));
                memset(&continuous_state->odometry, 0, sizeof(odometry2d_t));
            }
        }
    }
    continuous_state->odometry.pose.yaw = 0; 
}

void mobile_platform_proprioception_activity_resource_configuration(activity_t *activity)
{
    mobile_platform_proprioception_activity_resource_configuration_coordinate(activity);
    mobile_platform_proprioception_activity_resource_configuration_configure(activity);
    mobile_platform_proprioception_activity_resource_configuration_compute(activity);
}

// Pausing
void mobile_platform_proprioception_activity_pausing_coordinate(activity_t *activity)
{
    mobile_platform_proprioception_activity_coordination_state_t * coord_state = 
        (mobile_platform_proprioception_activity_coordination_state_t *) activity->state.coordination_state;

    // Coordinating with other activities
    if (*coord_state->execution_request)
        activity->state.lcsm_protocol = EXECUTION;
    if (*coord_state->deleting_request)
        activity->state.lcsm_protocol = DEINITIALISATION;

    // Coordinating own activity
    switch (activity->state.lcsm_protocol){ 
        case EXECUTION:
            activity->lcsm.state = RUNNING;
            break;
        case DEINITIALISATION:
            activity->lcsm.state = RESOURCE_CONFIGURATION;
            break;
    }
    update_super_state_lcsm_flags(&activity->state.lcsm_flags, activity->lcsm.state);
}

void mobile_platform_proprioception_activity_pausing_configure(activity_t *activity)
{
    if (activity->lcsm.state != PAUSING){
        // Update schedule
        add_schedule_to_eventloop(&activity->schedule_table, "activity_config");
        remove_schedule_from_eventloop(&activity->schedule_table, "pausing");
    }
}

void mobile_platform_proprioception_activity_pausing(activity_t *activity)
{
    mobile_platform_proprioception_activity_pausing_coordinate(activity);
    mobile_platform_proprioception_activity_pausing_configure(activity);
}

// Running
void mobile_platform_proprioception_activity_running_communicate_with_drive(activity_t *activity)
{
    mobile_platform_proprioception_activity_params_t *params = 
        (mobile_platform_proprioception_activity_params_t *) activity->conf.params;
    mobile_platform_proprioception_activity_coordination_state_t * coord_state = 
        (mobile_platform_proprioception_activity_coordination_state_t *) activity->state.coordination_state;
    
    // Copy config to a local buffer. Set mutex to null if read/write are in the same thread. This
    // will avoid an unncessary OS call to lock/unlock the mutex
    if(coord_state->drive_lock)
        mtx_lock(coord_state->drive_lock);
         
    memcpy(&params->drive.config, &params->rt_drive->config, sizeof(params->drive.config));
    memcpy(&params->drive.sensor, &params->rt_drive->sensor, sizeof(params->drive.sensor));

    if(coord_state->drive_lock)
        mtx_unlock(coord_state->drive_lock);

    // copy platform data to local buffer (static data)
    memcpy(&params->platform, params->rt_platform, sizeof(params->platform));
}

void mobile_platform_proprioception_activity_running_coordinate(activity_t *activity)
{
    mobile_platform_proprioception_activity_coordination_state_t* coord_state = (mobile_platform_proprioception_activity_coordination_state_t *) activity->state.coordination_state;
    if (*coord_state->deleting_request){
        // Update LCSM
        activity->lcsm.state = RESOURCE_CONFIGURATION;
        activity->state.lcsm_protocol = DEINITIALISATION;
        update_super_state_lcsm_flags(&activity->state.lcsm_flags, activity->lcsm.state);
    }
}

void mobile_platform_proprioception_activity_running_configure(activity_t *activity)
{
    if (activity->lcsm.state != RUNNING){
        // Update schedule
        add_schedule_to_eventloop(&activity->schedule_table, "activity_config");
        remove_schedule_from_eventloop(&activity->schedule_table, "running");
    }
}

void mobile_platform_proprioception_activity_running_compute(activity_t *activity)
{
    mobile_platform_proprioception_activity_params_t *params = 
        (mobile_platform_proprioception_activity_params_t *) activity->conf.params;
    mobile_platform_proprioception_activity_continuous_state_t *continuous_state = 
        (mobile_platform_proprioception_activity_continuous_state_t *) activity->state.computational_state.continuous;
    mobile_platform_proprioception_activity_discrete_state_t *discrete_state = 
        (mobile_platform_proprioception_activity_discrete_state_t *) activity->state.computational_state.discrete;
 
    timestamp_t timestamp = params->drive.sensor.timestamp; 
    double sampling_time_ms = 
        ((double) timestamp.seconds - continuous_state->previous_timestamp.seconds)*1000 +
        ((double) timestamp.nanoseconds - continuous_state->previous_timestamp.nanoseconds)/(1000*1000);
    if (sampling_time_ms < params->sampling_time_tolerance && sampling_time_ms > 0)
    {
        odometry_from_encoders_delta_tricycle(&params->platform, &params->drive, 
            sampling_time_ms, &continuous_state->odometry);
    }

    // printf("INSIDE: %f %f %f %f\n", 
    //     continuous_state->odometry.pose.x, continuous_state->odometry.pose.y, continuous_state->odometry.pose.yaw,
    //     continuous_state->odometry.velocity.vx);

    continuous_state->previous_timestamp = timestamp;
}    

void mobile_platform_proprioception_activity_running_communicate_odometry(activity_t *activity)
{
    mobile_platform_proprioception_activity_params_t *params = 
        (mobile_platform_proprioception_activity_params_t *) activity->conf.params;
    mobile_platform_proprioception_activity_continuous_state_t *continuous_state = 
        (mobile_platform_proprioception_activity_continuous_state_t *) activity->state.computational_state.continuous;
    mobile_platform_proprioception_activity_coordination_state_t *coord_state = 
        (mobile_platform_proprioception_activity_coordination_state_t *) activity->state.coordination_state;
    
    // Copy measurements to a local buffer (lock mutex if assigned)
    if(coord_state->odometry_lock)
        mtx_lock(coord_state->odometry_lock); 
    memcpy(params->rt_odometry, &continuous_state->odometry, sizeof(odometry2d_t));
    if(coord_state->odometry_lock)
        mtx_unlock(coord_state->odometry_lock);

}

void mobile_platform_proprioception_activity_running(activity_t *activity)
{
    mobile_platform_proprioception_activity_running_communicate_with_drive(activity);
    mobile_platform_proprioception_activity_running_coordinate(activity);
    mobile_platform_proprioception_activity_running_configure(activity);
    mobile_platform_proprioception_activity_running_compute(activity);
    mobile_platform_proprioception_activity_running_communicate_odometry(activity);
}

// Schedules
void mobile_platform_proprioception_activity_register_schedules(activity_t *activity)
{
    schedule_t schedule_config = {.number_of_functions = 0};
    register_function(&schedule_config, (function_ptr_t) mobile_platform_proprioception_activity_config, 
        activity, "activity_config");
    register_schedule(&activity->schedule_table, schedule_config, "activity_config");
    
    schedule_t schedule_resource_confiuration = {.number_of_functions = 0};
    register_function(&schedule_resource_confiuration, (function_ptr_t) mobile_platform_proprioception_activity_resource_configuration, 
        activity, "resource_configuration");
    register_schedule(&activity->schedule_table, schedule_resource_confiuration,
        "resource_configuration");

    schedule_t schedule_pausing = {.number_of_functions = 0};
    register_function(&schedule_pausing, (function_ptr_t) mobile_platform_proprioception_activity_pausing, 
        activity, "pausing");
    register_schedule(&activity->schedule_table, schedule_pausing, "pausing");

    schedule_t schedule_running = {.number_of_functions = 0};
    register_function(&schedule_running, (function_ptr_t) mobile_platform_proprioception_activity_running, 
        activity, "running");
    register_schedule(&activity->schedule_table, schedule_running, "running");

}

// LCSM 
void mobile_platform_proprioception_activity_create_lcsm(activity_t* activity, 
    const char* activity_name)
{
    // Allocate memory
    activity->conf.params = malloc(sizeof(mobile_platform_proprioception_activity_params_t));
    activity->state.computational_state.continuous = malloc(sizeof(mobile_platform_proprioception_activity_continuous_state_t));
    activity->state.computational_state.discrete = malloc(sizeof(mobile_platform_proprioception_activity_discrete_state_t));
    activity->state.coordination_state = malloc(sizeof(mobile_platform_proprioception_activity_coordination_state_t));

    // Initialise pointers
    mobile_platform_proprioception_activity_params_t *params = 
        (mobile_platform_proprioception_activity_params_t *) activity->conf.params;
    params->rt_drive = NULL;
    params->rt_platform = NULL;
    params->rt_odometry = NULL;
    mobile_platform_proprioception_activity_coordination_state_t *coord_state = 
        (mobile_platform_proprioception_activity_coordination_state_t *) activity->state.coordination_state;
    coord_state->deleting_request = NULL;
    coord_state->execution_request = NULL;
    coord_state->drive_lock = NULL;
    coord_state->odometry_lock = NULL;
}

void mobile_platform_proprioception_activity_configure_lcsm(activity_t* activity, 
    variable_table_t *variable_table)
{
    configure_lcsm_activity(activity, variable_table);
    // Select the inital state of LCSM and the LCSM protocol
    activity->lcsm.state = RESOURCE_CONFIGURATION;
    activity->state.lcsm_protocol = INITIALISATION;
   
    // Schedule table (adding config() for the first eventloop iteration)
    mobile_platform_proprioception_activity_register_schedules(activity);
    add_schedule_to_eventloop(&activity->schedule_table, "activity_config");
}

void mobile_platform_proprioception_activity_destroy_lcsm(activity_t* activity){
    destroy_activity(activity);
}

const activity_lcsm_t mobile_platform_proprioception_activity_lcsm =
{
    .create = mobile_platform_proprioception_activity_create_lcsm,
    .configure = mobile_platform_proprioception_activity_configure_lcsm,
    .destroy = mobile_platform_proprioception_activity_destroy_lcsm,
};


int mobile_platform_proprioception_activity_load_configuration_file(char *path_to_file,
    activity_t *activity)
{
    mobile_platform_proprioception_activity_params_t *params = 
        (mobile_platform_proprioception_activity_params_t *) activity->conf.params;
    mobile_platform_proprioception_activity_continuous_state_t *continuous_state = 
        (mobile_platform_proprioception_activity_continuous_state_t *) activity->state.computational_state.continuous;

    param_array_t param_array[] = {
        {"sampling_time_tolerance", &params->sampling_time_tolerance, PARAM_TYPE_DOUBLE},
        {"platform/wheel/pose/x", &params->platform.wheel.pose.x, PARAM_TYPE_DOUBLE},
        {"platform/wheel/pose/y", &params->platform.wheel.pose.y, PARAM_TYPE_DOUBLE},
        {"platform/wheel/pose/yaw", &params->platform.wheel.pose.yaw, PARAM_TYPE_DOUBLE},
        {"platform/length", &params->platform.length, PARAM_TYPE_DOUBLE},
        {"platform/width", &params->platform.width, PARAM_TYPE_DOUBLE}
    };
  
    int result;
    int number_of_parameters = sizeof(param_array)/sizeof(param_array_t);
    read_from_input_file(path_to_file, param_array, number_of_parameters, &result);

    return result;
}
