/* ----------------------------------------------------------------------------
 * Project Title,
 * ROB @ KU Leuven, Leuven, Belgium
 * Authors:
 * See LICENSE for the license information
 * -------------------------------------------------------------------------- */

/**
 * @file navigation_activity.c
 * @date March 18, 2024
 **/

#include <string.h>
#include <navigation/activity/navigation_activity.h>


/**
 * The config() has to be scheduled everytime a change in the LCSM occurs,
 * so it properly configures the schedule for the next iteration according
 * to the LCSM state, resources, task, ..
 * @param[in] activity data structure for the free space activity
 */
void navigation_activity_config(activity_t *activity)
{        
    // Remove config() from the eventloop schedule in the next iteration
    remove_schedule_from_eventloop(&activity->schedule_table, "activity_config");
    // Deciding which schedule to add
    switch (activity->lcsm.state) {
        case CREATION:
            add_schedule_to_eventloop(&activity->schedule_table, "creation");
            break;
        case RESOURCE_CONFIGURATION:
            add_schedule_to_eventloop(&activity->schedule_table, "resource_configuration");
            break;
        case CAPABILITY_CONFIGURATION:
            add_schedule_to_eventloop(&activity->schedule_table, "capability_configuration");
            break;
        case RUNNING:
            add_schedule_to_eventloop(&activity->schedule_table, "running");
            break;
        case CLEANING:
            break;
        case DONE:
            break;
    }
};

// Creation
void navigation_activity_creation_coordinate(activity_t *activity)
{
    navigation_activity_coordination_state_t *coord_state = (navigation_activity_coordination_state_t *)activity->state.coordination_state;
    // Coordinating own activity
    if (activity->state.lcsm_flags.creation_complete)
        activity->lcsm.state = RESOURCE_CONFIGURATION;
    update_super_state_lcsm_flags(&activity->state.lcsm_flags, activity->lcsm.state);
}

void navigation_activity_creation_configure(activity_t *activity)
{
    if (activity->lcsm.state != CREATION)
    {
        // Update schedule
        add_schedule_to_eventloop(&activity->schedule_table, "activity_config");
        remove_schedule_from_eventloop(&activity->schedule_table, "creation");
    }
}

void navigation_activity_creation_compute(activity_t *activity)
{
    // Set the flag below to true when the creation behaviour has finished
    navigation_activity_params_t *params = (navigation_activity_params_t *)activity->conf.params;
    navigation_activity_coordination_state_t *coord_state = (navigation_activity_coordination_state_t *)activity->state.coordination_state;
    navigation_activity_continuous_state_t *continuous_state = (navigation_activity_continuous_state_t *)activity->state.computational_state.continuous;

    // Initialize values
    params->des_platform_velocity.vx = 0;
    params->des_platform_velocity.wz = 0;

    // Initialize logging variables, logging not active in this activity!
    coord_state->actuation_logging_acquire = false;
    coord_state->actuation_current_chunk_index = 0;
    coord_state->actuation_logging_port = NULL;

    activity->state.lcsm_flags.creation_complete = true;
}

void navigation_activity_creation(activity_t *activity)
{
    navigation_activity_creation_compute(activity);
    navigation_activity_creation_coordinate(activity);
    navigation_activity_creation_configure(activity);
}

// Resource configuration
void navigation_activity_resource_configuration_coordinate(activity_t *activity)
{
    navigation_activity_coordination_state_t *coord_state = (navigation_activity_coordination_state_t *)activity->state.coordination_state;
    if (*coord_state->deleting_request)
    {
        activity->state.lcsm_protocol = DEINITIALISATION;
        activity->state.lcsm_flags.resource_configuration_complete = true;
    }

 
    // Internal coordination
    if (activity->state.lcsm_flags.resource_configuration_complete)
    {
        switch (activity->state.lcsm_protocol)
        {
        case INITIALISATION:
            activity->lcsm.state = CAPABILITY_CONFIGURATION;
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

void navigation_activity_resource_configuration_configure(activity_t *activity)
{
    if (activity->lcsm.state != RESOURCE_CONFIGURATION)
    {
        // Update schedule
        add_schedule_to_eventloop(&activity->schedule_table, "activity_config");
        remove_schedule_from_eventloop(&activity->schedule_table, "resource_configuration");
        // Update flags for next visit to the resource configuration LCS
        activity->state.lcsm_flags.resource_configuration_complete = false;
    }
}

void navigation_activity_resource_configuration_compute(activity_t *activity)
{
    navigation_activity_params_t *params = (navigation_activity_params_t *)activity->conf.params;
    navigation_activity_continuous_state_t *continuous_state = (navigation_activity_continuous_state_t *)
        activity->state.computational_state.continuous;
    navigation_activity_coordination_state_t *coord_state = (navigation_activity_coordination_state_t *)
        activity->state.coordination_state;

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
    if (params->rt_odometry == NULL)
    {
        hash_result_t hash_result;
        variable_registration_args_t registration_args = {
            .data ={.model= "odometry"}
        };
        GET_VARIABLE_IN_TABLE(activity->table, registration_args, 
            odometry2d_t, params->rt_odometry, coord_state->odometry_lock, 
            hash_result);
	if(hash_result == HASH_ENTRY_NOT_FOUND)
            activity->state.lcsm_flags.resource_configuration_complete = false;

    } 
    if (params->rt_range_sensor == NULL)
    {
        hash_result_t hash_result;
        variable_registration_args_t registration_args = {
            .data ={.model= "hokuyo"}
        };
        GET_VARIABLE_IN_TABLE(activity->table, registration_args, 
            range_sensor_t, params->rt_range_sensor, coord_state->range_sensor_lock, 
            hash_result);
        if(hash_result == HASH_ENTRY_NOT_FOUND)
            activity->state.lcsm_flags.resource_configuration_complete = false;
    } 
    if (params->rt_range_scan == NULL)
    {
        hash_result_t hash_result;
        variable_registration_args_t registration_args = {
            .data ={.model= "hokuyo"}
        };
        GET_VARIABLE_IN_TABLE(activity->table, registration_args, 
            range_scan_t, params->rt_range_scan, coord_state->range_scan_lock, 
            hash_result);
        if(hash_result == HASH_ENTRY_NOT_FOUND)
            activity->state.lcsm_flags.resource_configuration_complete = false;
        else{
            params->range_scan.angles = (double *) 
                malloc(params->rt_range_scan->nb_measurements*sizeof(double));
            params->range_scan.measurements = (double *) 
                malloc(params->rt_range_scan->nb_measurements*sizeof(double));
        }
    }
    if (params->rt_des_platform_velocity == NULL)
    {
        hash_result_t hash_result;
        variable_registration_args_t registration_args = {
            .data ={.model= "desired_platform_velocity"}
        };        
        GET_VARIABLE_IN_TABLE(activity->table, registration_args, 
            velocity_t, params->rt_des_platform_velocity, coord_state->des_platform_velocity_lock, 
            hash_result);
        if(hash_result == HASH_ENTRY_NOT_FOUND)
            activity->state.lcsm_flags.resource_configuration_complete = false;
    }
    if (params->rt_platform == NULL)
    {
        hash_result_t hash_result;
        variable_registration_args_t registration_args = {
            .data ={.model= "platform"}
        };
        GET_VARIABLE_IN_TABLE(activity->table, registration_args, 
            delta_tricycle_t, params->rt_platform, coord_state->platform_lock, hash_result);
	if(hash_result==HASH_ENTRY_NOT_FOUND)
            activity->state.lcsm_flags.resource_configuration_complete = false;
    } 
}

void navigation_activity_resource_configuration(activity_t *activity)
{
    navigation_activity_resource_configuration_compute(activity);
    navigation_activity_resource_configuration_coordinate(activity);
    navigation_activity_resource_configuration_configure(activity);
}

// Capability configuration
void navigation_activity_capability_configuration_communicate(activity_t *activity)
{
    navigation_activity_params_t *params = (navigation_activity_params_t *)activity->conf.params;
    navigation_activity_continuous_state_t *continuous_state = (navigation_activity_continuous_state_t *)activity->state.computational_state.continuous;
    navigation_activity_coordination_state_t *coord_state = (navigation_activity_coordination_state_t *)activity->state.coordination_state;

    // Copy LiDAR sensor data
    range_sensor_t *range_sensor = &params->range_sensor; 
    range_sensor_t *rt_range_sensor = params->rt_range_sensor; 

    mtx_lock(coord_state->range_sensor_lock);
    memcpy(range_sensor, rt_range_sensor, sizeof(range_sensor_t));
    mtx_unlock(coord_state->range_sensor_lock);
}

void navigation_activity_capability_configuration_coordinate(activity_t *activity)
{
    navigation_activity_coordination_state_t *coord_state = (navigation_activity_coordination_state_t *)
        activity->state.coordination_state;

    if (*coord_state->execution_request)
        activity->state.lcsm_protocol = EXECUTION;
    if (*coord_state->deleting_request)
    {
        activity->state.lcsm_protocol = DEINITIALISATION;
    }
    
    // Coordinating own activity
    switch (activity->state.lcsm_protocol)
    {
    case EXECUTION:
        activity->lcsm.state = RUNNING;
        break;
    case DEINITIALISATION:
        activity->lcsm.state = RESOURCE_CONFIGURATION;
        break;
    }
    update_super_state_lcsm_flags(&activity->state.lcsm_flags, activity->lcsm.state);
}

void navigation_activity_capability_configuration_configure(activity_t *activity)
{
    if (activity->lcsm.state != CAPABILITY_CONFIGURATION)
    {
        // Update schedule
        add_schedule_to_eventloop(&activity->schedule_table, "activity_config");
        remove_schedule_from_eventloop(&activity->schedule_table, "capability_configuration");
    }
}

void navigation_activity_capability_configuration_compute(activity_t *activity)
{
    navigation_activity_params_t *params = (navigation_activity_params_t *)activity->conf.params;
    navigation_activity_continuous_state_t *continuous_state = (navigation_activity_continuous_state_t *) 
        activity->state.computational_state.continuous;
    navigation_activity_coordination_state_t *coord_state = (navigation_activity_coordination_state_t *)
        activity->state.coordination_state;

}

void navigation_activity_capability_configuration(activity_t *activity)
{
    navigation_activity_capability_configuration_communicate(activity);
    navigation_activity_capability_configuration_compute(activity);
    navigation_activity_capability_configuration_coordinate(activity);
    navigation_activity_capability_configuration_configure(activity);
}

// Running
void navigation_activity_running_communicate_sensor_and_estimation(activity_t *activity)
{
    navigation_activity_params_t *params = (navigation_activity_params_t *)activity->conf.params;
    navigation_activity_continuous_state_t *continuous_state = (navigation_activity_continuous_state_t *)
        activity->state.computational_state.continuous;
    navigation_activity_coordination_state_t *coord_state = (navigation_activity_coordination_state_t *)
        activity->state.coordination_state;

    // Copy wheel's configuration and sensor data to a local buffer
    mtx_lock(coord_state->drive_lock); 
    memcpy(&params->drive.config, &params->rt_drive->config, sizeof(params->drive.config));
    memcpy(&params->drive.sensor, &params->rt_drive->sensor, sizeof(params->drive.sensor));
    mtx_unlock(coord_state->drive_lock);

    // Copy odometry data
    mtx_lock(coord_state->odometry_lock); 
    memcpy(&params->odometry, params->rt_odometry, sizeof(odometry2d_t));
    mtx_unlock(coord_state->odometry_lock);

    // Copy LiDAR data
    range_scan_t *range_scan = &params->range_scan;  
    range_sensor_t *range_sensor = &params->range_sensor; 
    range_scan_t *rt_range_scan = params->rt_range_scan;  
    range_sensor_t *rt_range_sensor = params->rt_range_sensor; 

    mtx_lock(coord_state->range_scan_lock);
    memcpy(range_sensor, rt_range_sensor, sizeof(range_sensor_t));
    range_scan->nb_measurements = rt_range_scan->nb_measurements;
    range_scan->timestamp = rt_range_scan->timestamp;
    memcpy(range_scan->angles, rt_range_scan->angles, range_scan->nb_measurements*sizeof(double));
    memcpy(range_scan->measurements, rt_range_scan->measurements, range_scan->nb_measurements*sizeof(double));
    mtx_unlock(coord_state->range_scan_lock);

    // Copy platform data
    mtx_lock(coord_state->platform_lock);
    memcpy(&params->platform, params->rt_platform, sizeof(params->platform));
    mtx_unlock(coord_state->platform_lock);
}

void navigation_activity_running_coordinate(activity_t *activity)
{
    navigation_activity_coordination_state_t *coord_state = (navigation_activity_coordination_state_t *)
        activity->state.coordination_state;

    // Coordinating with other activities
    if (*coord_state->deleting_request)
        activity->state.lcsm_protocol = DEINITIALISATION;

    switch (activity->state.lcsm_protocol)
    {
    case DEINITIALISATION:
        activity->lcsm.state = RESOURCE_CONFIGURATION;
        break;
    }
    update_super_state_lcsm_flags(&activity->state.lcsm_flags, activity->lcsm.state);
}

void navigation_activity_running_configure(activity_t *activity)
{
    navigation_activity_params_t *params = (navigation_activity_params_t *)activity->conf.params;
    if (activity->lcsm.state != RUNNING)
    {
        // Update schedule
        add_schedule_to_eventloop(&activity->schedule_table, "activity_config");
        remove_schedule_from_eventloop(&activity->schedule_table, "running");
    }
}

void navigation_activity_running_compute(activity_t *activity)
{
    navigation_activity_params_t *params = (navigation_activity_params_t *)activity->conf.params;
    navigation_activity_continuous_state_t *continuous_state = (navigation_activity_continuous_state_t *)
        activity->state.computational_state.continuous;

    // Drive data
    // https://gitlab.kuleuven.be/rob-ecs/ecs-2025/-/blob/main/Software/mobile_platform_5c/external/data_structure/include/drive_data_structure/differential_drive.h
    differential_drive_t *drive = &params->drive;
    // Range data 
    // https://gitlab.kuleuven.be/rob-ecs/ecs-2025/-/blob/main/Software/mobile_platform_5c/external/data_structure/include/sensor_data_structure/range_sensor.h
    range_sensor_t *range_sensor = &params->range_sensor;
    // https://gitlab.kuleuven.be/rob-ecs/ecs-2025/-/blob/main/Software/mobile_platform_5c/external/data_structure/include/sensor_data_structure/range_scan.h
    range_scan_t *range_scan = &params->range_scan;
    // Odometry 
    // https://gitlab.kuleuven.be/rob-ecs/ecs-2025/-/blob/main/Software/mobile_platform_5c/external/data_structure/include/navigation_data_structure/odometry2d.h
    odometry2d_t *odometry = &params->odometry;
    // Platform
    // https://gitlab.kuleuven.be/rob-ecs/ecs-2025/-/blob/main/Software/mobile_platform_5c/external/data_structure/include/platform_data_structure/delta_tricycle.h
    delta_tricycle_t *platform = &params->platform;
    
    // Desired platform velocity (to be computed in this activity!)
    // https://gitlab.kuleuven.be/rob-ecs/ecs-2025/-/blob/main/Software/mobile_platform_5c/external/data_structure/include/mechanics_data_structure/velocity.h
    velocity_t *desired_platform_velocity = &params->des_platform_velocity;



    // Implement some navigation behaviour here..
    // ...
    
    
    // Do not forget to update the values pointed by
    // "desired_platform_velocity".
    desired_platform_velocity->vx = 0;
    desired_platform_velocity->wz = 0;

}

void navigation_activity_running_communicate_control(activity_t *activity)
{
    navigation_activity_params_t *params = (navigation_activity_params_t *)activity->conf.params;
    navigation_activity_coordination_state_t *coord_state = (navigation_activity_coordination_state_t *)
        activity->state.coordination_state;

    // Send velocity to the robot..
    mtx_lock(coord_state->des_platform_velocity_lock);
    memcpy(params->rt_des_platform_velocity, &params->des_platform_velocity, sizeof(velocity_t));
    mtx_unlock(coord_state->des_platform_velocity_lock);

}

void navigation_activity_running(activity_t *activity)
{
    navigation_activity_running_communicate_sensor_and_estimation(activity);
    navigation_activity_running_coordinate(activity);
    navigation_activity_running_configure(activity);
    navigation_activity_running_compute(activity);
    navigation_activity_running_communicate_control(activity);
}

// SCHEDULER
void navigation_activity_register_schedules(activity_t *activity)
{
    schedule_t schedule_config = {.number_of_functions = 0};
    register_function(&schedule_config, (function_ptr_t)navigation_activity_config,
                      activity, "activity_config");
    register_schedule(&activity->schedule_table, schedule_config, "activity_config");

    schedule_t schedule_creation = {.number_of_functions = 0};
    register_function(&schedule_creation, (function_ptr_t)navigation_activity_creation,
                      activity, "creation");
    register_schedule(&activity->schedule_table, schedule_creation, "creation");

    schedule_t schedule_resource_configuration = {.number_of_functions = 0};
    register_function(&schedule_resource_configuration, (function_ptr_t)navigation_activity_resource_configuration,
                      activity, "resource_configuration");
    register_schedule(&activity->schedule_table, schedule_resource_configuration,
                      "resource_configuration");

    schedule_t schedule_capability_configuration = {.number_of_functions = 0};
    register_function(&schedule_capability_configuration, (function_ptr_t)navigation_activity_capability_configuration,
                      activity, "capability_configuration");
    register_schedule(&activity->schedule_table, schedule_capability_configuration,
                      "capability_configuration");

    schedule_t schedule_running = {.number_of_functions = 0};
    register_function(&schedule_running, (function_ptr_t)navigation_activity_running,
                      activity, "running");
    register_schedule(&activity->schedule_table, schedule_running,
                      "running");
}

void navigation_activity_create_lcsm(activity_t *activity, const char *activity_name)
{
    activity->conf.params = malloc(sizeof(navigation_activity_params_t));
    activity->state.computational_state.continuous = malloc(sizeof(navigation_activity_continuous_state_t));
    activity->state.computational_state.discrete = malloc(sizeof(navigation_activity_discrete_state_t));
    activity->state.coordination_state = malloc(sizeof(navigation_activity_coordination_state_t));

    // Initializing pointers to be configured for shared memory
    navigation_activity_params_t *params = (navigation_activity_params_t *)activity->conf.params;
    params->rt_range_scan = NULL;
    params->rt_range_sensor = NULL;
    params->rt_drive = NULL;
    params->rt_odometry = NULL;
    params->rt_des_platform_velocity = NULL;
    params->rt_platform = NULL;

    navigation_activity_coordination_state_t *coord_state = 
        (navigation_activity_coordination_state_t *) activity->state.coordination_state;
    coord_state->range_scan_lock = NULL;
    coord_state->range_sensor_lock = NULL;
    coord_state->des_platform_velocity_lock = NULL;
    coord_state->odometry_lock = NULL;
    coord_state->drive_lock = NULL;
    coord_state->platform_lock = NULL;
}

void navigation_activity_configure_lcsm(activity_t* activity, 
    variable_table_t *variable_table)
{
    configure_lcsm_activity(activity, variable_table);
    // Select the inital state of LCSM for this activity
    activity->lcsm.state = CREATION;
    activity->state.lcsm_protocol = INITIALISATION;

    // Schedule table (adding config() for the first eventloop iteration)
    navigation_activity_register_schedules(activity);
    add_schedule_to_eventloop(&activity->schedule_table, "activity_config");
}

void navigation_activity_destroy_lcsm(activity_t *activity)
{
    destroy_activity(activity);
}

const activity_lcsm_t navigation_activity_lcsm = {
    .create = navigation_activity_create_lcsm,
    .configure = navigation_activity_configure_lcsm,
    .destroy = navigation_activity_destroy_lcsm,
};

