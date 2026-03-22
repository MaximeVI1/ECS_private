/* ----------------------------------------------------------------------------
 * Project Title,
 * ROB @ KU Leuven, Leuven, Belgium
 * Authors:
 * See LICENSE for the license information
 * -------------------------------------------------------------------------- */

/**
 * @file navigation_activity.c
 * last update: March 9, 2026
 **/

#include <string.h>
#include <navigation/activity/navigation_activity.h>

// --- EXPERIMENT CONFIGURATION ---
// Verander deze waarden om het experiment aan te passen.
#define EXPERIMENT_DURATION 10.0               // Run time in seconds
#define EXPERIMENT_USE_VELOCITY_CONTROL true   // Set to false for the torque control experiment
#define EXPERIMENT_DESIRED_VELOCITY 0.5        // rad/s (velocity control reference)
#define EXPERIMENT_DESIRED_CURRENT 2.0         // Amps (torque control reference)
// --------------------------------

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
    navigation_activity_coordination_state_t *coord_state = 
        (navigation_activity_coordination_state_t *) activity->state.coordination_state;
    // Coordinating own activity
    if (activity->state.lcsm_flags.creation_complete) {
        activity->lcsm.state = RESOURCE_CONFIGURATION;
    }
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
    navigation_activity_coordination_state_t *coord_state = 
        (navigation_activity_coordination_state_t *) activity->state.coordination_state;
    navigation_activity_continuous_state_t *continuous_state = 
        (navigation_activity_continuous_state_t *) activity->state.computational_state.continuous;

    // Set logging counter to 0
    params->actuation_chunk_id = 0;
    continuous_state->iteration_count = 0;

    activity->state.lcsm_flags.creation_complete = true;
}

// Resource configuration
void navigation_activity_resource_configuration_coordinate(activity_t *activity)
{
    navigation_activity_coordination_state_t *coord_state = 
        (navigation_activity_coordination_state_t *)activity->state.coordination_state;
    
    if (*coord_state->deleting_request) {
        activity->state.lcsm_protocol = DEINITIALISATION;
        activity->state.lcsm_flags.resource_configuration_complete = true;
    }
 
    // Internal coordination
    if (activity->state.lcsm_flags.resource_configuration_complete) {
        switch (activity->state.lcsm_protocol) {
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
    }
}

void navigation_activity_resource_configuration_configure(activity_t *activity)
{
    if (activity->lcsm.state != RESOURCE_CONFIGURATION) {
        // Update schedule
        add_schedule_to_eventloop(&activity->schedule_table, "activity_config");
        remove_schedule_from_eventloop(&activity->schedule_table, "resource_configuration");
        
        // Update flags for next visit to the resource configuration LCS
        activity->state.lcsm_flags.resource_configuration_complete = false;
    }
}

void navigation_activity_resource_configuration_compute(activity_t *activity)
{
    navigation_activity_params_t *params = 
        (navigation_activity_params_t *)activity->conf.params;
    navigation_activity_continuous_state_t *continuous_state = 
        (navigation_activity_continuous_state_t *)activity->state.computational_state.continuous;
    navigation_activity_coordination_state_t *coord_state = 
        (navigation_activity_coordination_state_t *)activity->state.coordination_state;

    activity->state.lcsm_flags.resource_configuration_complete = true;

    // Get shared drive from shared memory.  
    if (params->rt_drive == NULL) {
        hash_result_t hash_result;
        variable_registration_args_t drive_registration_args = {
            .data ={.model= "kelo_drive"}
        };
        GET_VARIABLE_IN_TABLE(activity->table, drive_registration_args, 
            differential_drive_t, params->rt_drive, coord_state->drive_lock, 
            hash_result);
        if (hash_result == HASH_ENTRY_NOT_FOUND) {
            activity->state.lcsm_flags.resource_configuration_complete = false;
        }
    }

    // Get shared odometry from shared memory.
    if (params->rt_odometry == NULL) {
        hash_result_t hash_result;
        variable_registration_args_t registration_args = {
            .data ={.model= "odometry"}
        };
        GET_VARIABLE_IN_TABLE(activity->table, registration_args, 
            odometry2d_t, params->rt_odometry, coord_state->odometry_lock, 
            hash_result);
        if (hash_result == HASH_ENTRY_NOT_FOUND) {
            activity->state.lcsm_flags.resource_configuration_complete = false;
        }
    }
}

// Capability configuration
void navigation_activity_capability_configuration_coordinate(activity_t *activity)
{
    navigation_activity_coordination_state_t *coord_state = (navigation_activity_coordination_state_t *)
        activity->state.coordination_state;

    if (*coord_state->execution_request) {
        activity->state.lcsm_protocol = EXECUTION;
    }
    if (*coord_state->deleting_request) {
        activity->state.lcsm_protocol = DEINITIALISATION;
    }
    
    // Coordinating own activity
    switch (activity->state.lcsm_protocol) {
        case EXECUTION:
            activity->lcsm.state = RUNNING;
            break;
        case DEINITIALISATION:
            activity->lcsm.state = RESOURCE_CONFIGURATION;
            break;
    }
}

void navigation_activity_capability_configuration_configure(activity_t *activity)
{
    if (activity->lcsm.state != CAPABILITY_CONFIGURATION) {
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

    /* STUDENTS TODO: add capability configuration code here */
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
}

void navigation_activity_running_coordinate(activity_t *activity)
{
    navigation_activity_coordination_state_t *coord_state = (navigation_activity_coordination_state_t *)
        activity->state.coordination_state;

    // Coordinating with other activities
    if (*coord_state->deleting_request) {
        activity->state.lcsm_protocol = DEINITIALISATION;
    }

    switch (activity->state.lcsm_protocol) {
        case DEINITIALISATION:
            activity->lcsm.state = RESOURCE_CONFIGURATION;
            break;
    }
}

void navigation_activity_running_configure(activity_t *activity)
{
    navigation_activity_params_t *params = (navigation_activity_params_t *)activity->conf.params;
    if (activity->lcsm.state != RUNNING) {
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
    // Odometry 
    // https://gitlab.kuleuven.be/rob-ecs/ecs-2025/-/blob/main/Software/mobile_platform_5c/external/data_structure/include/navigation_data_structure/odometry2d.h
    odometry2d_t *odometry = &params->odometry;
    
    const double dt = 0.050;  // 50 ms (matches configuration)
    double elapsed_time = continuous_state->iteration_count * dt;

    if (elapsed_time < EXPERIMENT_DURATION){

        if (EXPERIMENT_USE_VELOCITY_CONTROL){
            drive->actuation.mode = DIFFERENTIAL_DRIVE_COMMAND_MODE_VELOCITY;
            drive->actuation.velocity.left_wheel  = EXPERIMENT_DESIRED_VELOCITY;
            drive->actuation.velocity.right_wheel = EXPERIMENT_DESIRED_VELOCITY;
            drive->actuation.current.left_wheel   = 0;
            drive->actuation.current.right_wheel  = 0;
        }

        else{
            drive->actuation.mode = DIFFERENTIAL_DRIVE_COMMAND_MODE_TORQUE;
            drive->actuation.current.left_wheel   = EXPERIMENT_DESIRED_CURRENT;
            drive->actuation.current.right_wheel  = EXPERIMENT_DESIRED_CURRENT;
            drive->actuation.velocity.left_wheel  = 0;
            drive->actuation.velocity.right_wheel = 0;
        }
        continuous_state->iteration_count++;

    }
    
    
}

void navigation_activity_running_communicate_control(activity_t *activity)
{
    navigation_activity_params_t *params = (navigation_activity_params_t *)activity->conf.params;
    navigation_activity_coordination_state_t *coord_state = (navigation_activity_coordination_state_t *)
        activity->state.coordination_state;

    // Send velocity to kelo drive..
    mtx_lock(coord_state->drive_lock); 
    memcpy(&params->rt_drive->actuation, &params->drive.actuation, sizeof(params->drive.actuation));
    mtx_unlock(coord_state->drive_lock);    

}

void navigation_activity_running_logging(activity_t *activity)
{
    navigation_activity_params_t *params = 
        (navigation_activity_params_t *)activity->conf.params;
    navigation_activity_coordination_state_t *coord_state = 
        (navigation_activity_coordination_state_t *) activity->state.coordination_state;

    lttng_ust_tracepoint(logging, drive_actuation_trace, 
        params->actuation_chunk_id, &params->drive.actuation);
    params->actuation_chunk_id++;
}

// SCHEDULER
void navigation_activity_register_schedules(activity_t *activity)
{
    schedule_t schedule_config = {.number_of_functions = 0};
    register_function(&schedule_config, (function_ptr_t)navigation_activity_config,
                      activity, "activity_config");
    register_schedule(&activity->schedule_table, schedule_config, "activity_config");

    schedule_t schedule_creation = {.number_of_functions = 0};
    register_function(&schedule_creation, (function_ptr_t)navigation_activity_creation_compute,
                      activity, "creation");
    register_function(&schedule_creation, (function_ptr_t)navigation_activity_creation_coordinate,
                      activity, "creation");
    register_function(&schedule_creation, (function_ptr_t)navigation_activity_creation_configure,
                      activity, "creation");
    register_schedule(&activity->schedule_table, schedule_creation, "creation");

    schedule_t schedule_resource_configuration = {.number_of_functions = 0};
    register_function(&schedule_resource_configuration, (function_ptr_t)navigation_activity_resource_configuration_compute,
                      activity, "resource_configuration");
    register_function(&schedule_resource_configuration, (function_ptr_t)navigation_activity_resource_configuration_coordinate,
                      activity, "resource_configuration");
    register_function(&schedule_resource_configuration, (function_ptr_t)navigation_activity_resource_configuration_configure,
                      activity, "resource_configuration");
    register_schedule(&activity->schedule_table, schedule_resource_configuration,
                      "resource_configuration");

    schedule_t schedule_capability_configuration = {.number_of_functions = 0};
    register_function(&schedule_capability_configuration, (function_ptr_t)navigation_activity_capability_configuration_compute,
                      activity, "capability_configuration");
    register_function(&schedule_capability_configuration, (function_ptr_t)navigation_activity_capability_configuration_coordinate,
                      activity, "capability_configuration");
    register_function(&schedule_capability_configuration, (function_ptr_t)navigation_activity_capability_configuration_configure,
                      activity, "capability_configuration");
    register_schedule(&activity->schedule_table, schedule_capability_configuration,
                      "capability_configuration");

    schedule_t schedule_running = {.number_of_functions = 0};
    register_function(&schedule_running, (function_ptr_t)navigation_activity_running_communicate_sensor_and_estimation,
                      activity, "running");
    register_function(&schedule_running, (function_ptr_t)navigation_activity_running_coordinate,
                      activity, "running");
    register_function(&schedule_running, (function_ptr_t)navigation_activity_running_configure,
                      activity, "running");
    register_function(&schedule_running, (function_ptr_t)navigation_activity_running_compute,
                      activity, "running");
    register_function(&schedule_running, (function_ptr_t)navigation_activity_running_communicate_control,
                      activity, "running");
    register_function(&schedule_running, (function_ptr_t)navigation_activity_running_logging,
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
    params->rt_odometry = NULL;
    params->rt_platform = NULL;

    navigation_activity_coordination_state_t *coord_state = 
        (navigation_activity_coordination_state_t *) activity->state.coordination_state;
    coord_state->drive_lock = NULL;
    coord_state->odometry_lock = NULL;
    coord_state->range_scan_lock = NULL;
    coord_state->range_sensor_lock = NULL;
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

