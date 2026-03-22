/* ----------------------------------------------------------------------------
 * Wheel drives using 5C template
 * ROB @ KU Leuven, Leuven, Belgium
 * See LICENSE for the license information
 * -------------------------------------------------------------------------- */

/**
 * @file kelo_drive_activity.c
 * @date October 12, 2021.
 **/

#include <mobile_platform_drive/activity/kelo_drive_activity.h>
#include <string.h>

/**
 * The config() has to be scheduled everytime a change in the LCSM occurs,
 * so it properly configures the schedule for the next iteration according
 * to the LCSM state, resources, task, ..
 * @param[in] activity data structure for the kelo activity
 */
void mobile_platform_drive_kelo_activity_config(activity_t *activity)
{
    // Remove config() from the eventloop schedule in the next iteration
    remove_schedule_from_eventloop(&activity->schedule_table, "activity_config");
    // Deciding which schedule to add
    switch (activity->lcsm.state)
    {
    case CREATION:
        add_schedule_to_eventloop(&activity->schedule_table, "creation");
        break;
    case RESOURCE_CONFIGURATION:
        add_schedule_to_eventloop(&activity->schedule_table, "resource_configuration");
        break;
    case CAPABILITY_CONFIGURATION:
        add_schedule_to_eventloop(&activity->schedule_table, "capability_configuration");
        break;
    case PAUSING:
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
void mobile_platform_drive_kelo_activity_creation_coordinate(activity_t *activity)
{
    // Coordinating own activity
    if (activity->state.lcsm_flags.creation_complete)
    {
        activity->lcsm.state = RESOURCE_CONFIGURATION;
        update_super_state_lcsm_flags(&activity->state.lcsm_flags, activity->lcsm.state);
    }
}

void mobile_platform_drive_kelo_activity_creation_configure(activity_t *activity)
{
    if (activity->lcsm.state != CREATION)
    {
        // Update schedule
        add_schedule_to_eventloop(&activity->schedule_table, "activity_config");
        remove_schedule_from_eventloop(&activity->schedule_table, "creation");
    }
}

void mobile_platform_drive_kelo_activity_creation_compute(activity_t *activity)
{
    mobile_platform_drive_kelo_activity_coordination_state_t *coord_state = 
        (mobile_platform_drive_kelo_activity_coordination_state_t *) activity->state.coordination_state;
    mobile_platform_drive_kelo_activity_discrete_state_t *discrete_state = 
        (mobile_platform_drive_kelo_activity_discrete_state_t *) activity->state.computational_state.discrete;
    mobile_platform_drive_kelo_activity_continuous_state_t *continuous_state = 
        (mobile_platform_drive_kelo_activity_continuous_state_t *) activity->state.computational_state.continuous;
    mobile_platform_drive_kelo_activity_params_t *params = 
        (mobile_platform_drive_kelo_activity_params_t *) activity->conf.params;

    // Unless otherwise stated..
    activity->state.lcsm_flags.creation_complete = true;

    // Initialize values
    discrete_state->number_of_connection_attempts = 0;
    params->max_number_of_connection_attempts = 0;

    // Allocate memory if needed
    if (coord_state->configuration_request == NULL)
    {
        coord_state->configuration_request = (bool *) malloc(sizeof(bool));
        // Did it fail to allocate memory?
        if (coord_state->configuration_request == NULL) 
            activity->state.lcsm_flags.creation_complete = false;
        else
            *coord_state->configuration_request=false;
    }

    // Set logging counter to 0
    params->sensor_chunk_id = 0;
}

void mobile_platform_drive_kelo_activity_creation(activity_t *activity)
{
    mobile_platform_drive_kelo_activity_creation_compute(activity);
    mobile_platform_drive_kelo_activity_creation_coordinate(activity);
    mobile_platform_drive_kelo_activity_creation_configure(activity);
}

// Resource configuration
void mobile_platform_drive_kelo_activity_resource_configuration_coordinate(activity_t *activity)
{
    mobile_platform_drive_kelo_activity_coordination_state_t *coord_state = (mobile_platform_drive_kelo_activity_coordination_state_t *)activity->state.coordination_state;

    if (*coord_state->deleting_request)
        activity->state.lcsm_protocol = DEINITIALISATION;

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
    }else if(activity->state.lcsm_flags.resource_configuration_failed)
    {
        activity->lcsm.state = DONE;
        activity->state.lcsm_flags.deletion_complete = true;
        update_super_state_lcsm_flags(&activity->state.lcsm_flags, activity->lcsm.state);
    }
}

void mobile_platform_drive_kelo_activity_resource_configuration_configure(activity_t *activity)
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

void mobile_platform_drive_kelo_activity_resource_configuration_compute(activity_t *activity)
{
    mobile_platform_drive_kelo_activity_params_t *params = 
        (mobile_platform_drive_kelo_activity_params_t *)activity->conf.params;
    mobile_platform_drive_kelo_activity_continuous_state_t *continuous_state = 
        (mobile_platform_drive_kelo_activity_continuous_state_t *)activity->state.computational_state.continuous;
    mobile_platform_drive_kelo_activity_discrete_state_t *discrete_state = 
        (mobile_platform_drive_kelo_activity_discrete_state_t *)activity->state.computational_state.discrete;
    mobile_platform_drive_kelo_activity_coordination_state_t *coord_state = 
        (mobile_platform_drive_kelo_activity_coordination_state_t *)activity->state.coordination_state;
    kelo_soem_state_t *kelo_soem_state = (kelo_soem_state_t *)&continuous_state->kelo_soem_state;
    int config_result;
    switch (activity->state.lcsm_protocol)
    {
    case INITIALISATION:
        configure_mobile_platform_drive_kelo_activity_from_file(params->configuration_file,
            params, &config_result);
        continuous_state->clocktime_at_last_connection_attempt = *(activity->clocktime);
        discrete_state->number_of_connection_attempts++;
        discrete_state->ethercat_master_connected = false;
        discrete_state->ethercat_master_connected = initialise_ethercat_master(
            &continuous_state->kelo_soem_state, &params->kelo_soem_params);

        if (discrete_state->ethercat_master_connected)
        {
            // Initialize values actually sent to the wheels
            for (int i = 0; i < kelo_soem_state->nb_smart_wheel_detected; i++)
            {
                // Initialize values that are actually sent to the drive
                kelo_soem_state->cmd_data_smart_wheel[i]->command1 = 0x00;
                kelo_soem_state->cmd_data_smart_wheel[i]->command2 = 0x00;
                kelo_soem_state->cmd_data_smart_wheel[i]->setpoint1 = 0.0;
                kelo_soem_state->cmd_data_smart_wheel[i]->setpoint2 = 0.0;
                kelo_soem_state->cmd_data_smart_wheel[i]->limit1_p = 4.0;
                kelo_soem_state->cmd_data_smart_wheel[i]->limit1_n = -4.0;
                kelo_soem_state->cmd_data_smart_wheel[i]->limit2_p = 4.0;
                kelo_soem_state->cmd_data_smart_wheel[i]->limit2_n = -4.0;
            }

            // Initialise local variable of the drives
            for (int i = 0; i < kelo_soem_state->nb_smart_wheel_detected; i++)
            {
              	params->drive[i].actuation.mode = 0x00;
                params->drive[i].actuation.velocity.left_wheel = 0;
                params->drive[i].actuation.velocity.right_wheel = 0;
                params->drive[i].actuation.current.left_wheel = 0;
                params->drive[i].actuation.current.right_wheel = 0;
            }

            // Register and initialise copy of drive in shared memory
            for (int i = 0; i < kelo_soem_state->nb_smart_wheel_detected; i++)
            {
                hash_result_t hash_result;
                // SID is the name associated with SW serial number of kelo
                char SID[50] = "";
                bool in_array;
                for(int search_index = 0; search_index < MAX_NUMBER_SMARTWHEELS; search_index++){
                    in_array = (kelo_soem_state->sw_serial_number[i] == params->id[search_index].sw_serial_number);
                    if(in_array){
                        strcpy(SID, params->id[search_index].name);
                        break;
                    }
                }

                if(!in_array){
                    printf("[DRIVE ACTIVITY] Kelo number %d in etherCAT chain has unknown \"SW serial number\": %d\n"
                            "[DRIVE ACTIVITY] Please, update the drive configuration file with this number.\n", i+1, 
                            kelo_soem_state->sw_serial_number[i]);
                    return;
                }

                variable_registration_args_t reg_args = {
                    .data={.model=""}
                };
                strcpy(reg_args.data.model, SID);   // SID of the data entry
                REGISTER_VARIABLE_IN_TABLE(activity->table, reg_args, differential_drive_t, hash_result);
                GET_VARIABLE_IN_TABLE(activity->table, reg_args, differential_drive_t, params->rt_drive[i], 
                    coord_state->drive_lock[i], hash_result);
                
                // Check if succeeded
                if(hash_result == HASH_ENTRY_NOT_FOUND)
                {   
                    printf("[Kelo drive activity] Failed to find differential_drive_t \
                        entry on the table of wheel #%d\n", i);
                    return;
                }

                // Initialize configuration values
                params->rt_drive[i]->config = params->drive[i].config;
                // Initialize values in message that is shared among activities
                params->rt_drive[i]->actuation.mode = DIFFERENTIAL_DRIVE_COMMAND_MODE_VELOCITY;
                params->rt_drive[i]->actuation.velocity.left_wheel = 0;
                params->rt_drive[i]->actuation.velocity.right_wheel = 0;
                params->rt_drive[i]->actuation.current.left_wheel = 0;
                params->rt_drive[i]->actuation.current.right_wheel = 0;
            }

            printf("[Kelo activity] Connected to %d Kelo wheel(s)\n",
                   continuous_state->kelo_soem_state.nb_smart_wheel_detected);
            activity->state.lcsm_flags.resource_configuration_complete = true;
        }
        else
        {
            printf("[Kelo activity] Failed to connected to Kelo wheel. Have you tried sudo setcap cap_net_raw+ep <AppName>?\n");
            if(discrete_state->number_of_connection_attempts >= params->max_number_of_connection_attempts){
                activity->state.lcsm_flags.resource_configuration_failed = true;
            }
        }
        break;

    case DEINITIALISATION:
        //shutdown_ethercat_master(&continuous_state->kelo_soem_state, &params->kelo_soem_params);
        activity->state.lcsm_flags.resource_configuration_complete = true;
        printf("[Kelo activity] Kelo deinitialised\n");
        break;
    }
}

void mobile_platform_drive_kelo_activity_resource_configuration(activity_t *activity)
{
    mobile_platform_drive_kelo_activity_params_t *params = 
        (mobile_platform_drive_kelo_activity_params_t *)activity->conf.params;
    mobile_platform_drive_kelo_activity_continuous_state_t *continuous_state = 
        (mobile_platform_drive_kelo_activity_continuous_state_t *)activity->state.computational_state.continuous;

    // compute time since last connection attempt (performed at compute)
    float time_elapsed_since_last_connection_attempt = 
        (float) (activity->clocktime->tv_sec - continuous_state->clocktime_at_last_connection_attempt.tv_sec +
            0.000000001*(activity->clocktime->tv_nsec - continuous_state->clocktime_at_last_connection_attempt.tv_nsec) );
    
    if(time_elapsed_since_last_connection_attempt > params->min_time_between_connection_attempts){
        mobile_platform_drive_kelo_activity_resource_configuration_compute(activity);
    }
    mobile_platform_drive_kelo_activity_resource_configuration_coordinate(activity);
    mobile_platform_drive_kelo_activity_resource_configuration_configure(activity);
}

// Capability configuration
void mobile_platform_drive_kelo_activity_capability_configuration_communicate(activity_t *activity)
{
    mobile_platform_drive_kelo_activity_continuous_state_t *continuous_state =
        (mobile_platform_drive_kelo_activity_continuous_state_t *)activity->state.computational_state.continuous;
    mobile_platform_drive_kelo_activity_params_t *params = 
        (mobile_platform_drive_kelo_activity_params_t *)activity->conf.params;
    mobile_platform_drive_kelo_activity_coordination_state_t *coord_state = \
        (mobile_platform_drive_kelo_activity_coordination_state_t *)activity->state.coordination_state;

    kelo_soem_state_t *kelo_soem_state = (kelo_soem_state_t *)&continuous_state->kelo_soem_state;

    for (int i = 0; i < kelo_soem_state->nb_smart_wheel_detected; i++)
    {
        mtx_lock(coord_state->drive_lock[i]);
        params->drive[i].actuation.mode = params->rt_drive[i]->actuation.mode;
        mtx_unlock(coord_state->drive_lock[i]);
    }
}

void mobile_platform_drive_kelo_activity_capability_configuration_coordinate(activity_t *activity)
{
    mobile_platform_drive_kelo_activity_coordination_state_t *coord_state = 
        (mobile_platform_drive_kelo_activity_coordination_state_t *) activity->state.coordination_state;
    // Coordinating with other activities
    if (*coord_state->execution_request)
        activity->state.lcsm_protocol = EXECUTION;
    if (*coord_state->deleting_request)
        activity->state.lcsm_protocol = DEINITIALISATION;

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

void mobile_platform_drive_kelo_activity_capability_configuration_configure(activity_t *activity)
{
    if (activity->lcsm.state != CAPABILITY_CONFIGURATION)
    {
        // Update schedule
        add_schedule_to_eventloop(&activity->schedule_table, "activity_config");
        remove_schedule_from_eventloop(&activity->schedule_table, "capability_configuration");
    }
}

void mobile_platform_drive_kelo_activity_capability_configuration_compute(activity_t *activity)
{
    mobile_platform_drive_kelo_activity_params_t *params = (mobile_platform_drive_kelo_activity_params_t *)activity->conf.params;
    mobile_platform_drive_kelo_activity_continuous_state_t *continuous_state = (mobile_platform_drive_kelo_activity_continuous_state_t *)activity->state.computational_state.continuous;

    // Changing wheel mode
    kelo_soem_state_t *kelo_soem_state = (kelo_soem_state_t *)&continuous_state->kelo_soem_state;
    for (int i = 0; i < kelo_soem_state->nb_smart_wheel_detected; i++)
    {
        switch (params->drive[i].actuation.mode)
        {
        case DIFFERENTIAL_DRIVE_COMMAND_MODE_TORQUE:
            kelo_soem_state->cmd_data_smart_wheel[i]->command1 = COM1_MODE_TORQUE;
            break;
        case DIFFERENTIAL_DRIVE_COMMAND_MODE_VELOCITY:
            kelo_soem_state->cmd_data_smart_wheel[i]->command1 = COM1_MODE_VELOCITY;
            break;
        default:
            kelo_soem_state->cmd_data_smart_wheel[i]->command1 = 0x00;
            kelo_soem_state->cmd_data_smart_wheel[i]->setpoint1 = 0.0;
            kelo_soem_state->cmd_data_smart_wheel[i]->setpoint2 = 0.0;
            break;
        }

    }
}

void mobile_platform_drive_kelo_activity_capability_configuration(activity_t *activity)
{
    mobile_platform_drive_kelo_activity_capability_configuration_communicate(activity);
    mobile_platform_drive_kelo_activity_capability_configuration_coordinate(activity);
    mobile_platform_drive_kelo_activity_capability_configuration_configure(activity);
    mobile_platform_drive_kelo_activity_capability_configuration_compute(activity);
}

// Running
void mobile_platform_drive_kelo_activity_running_communicate(activity_t *activity)
{
    mobile_platform_drive_kelo_activity_params_t *params = 
        (mobile_platform_drive_kelo_activity_params_t *)activity->conf.params;
    mobile_platform_drive_kelo_activity_continuous_state_t *continuous_state = 
        (mobile_platform_drive_kelo_activity_continuous_state_t *) activity->state.computational_state.continuous;
    mobile_platform_drive_kelo_activity_coordination_state_t *coord_state = 
        (mobile_platform_drive_kelo_activity_coordination_state_t *) activity->state.coordination_state;
    kelo_soem_state_t *kelo_soem_state = (kelo_soem_state_t *) &continuous_state->kelo_soem_state;

    for (int i = 0; i < kelo_soem_state->nb_smart_wheel_detected; i++)
    {
        mtx_lock(coord_state->drive_lock[i]);
        if (params->rt_drive[i]->actuation.mode == params->drive[i].actuation.mode)
        {
            params->drive[i].actuation.velocity = params->rt_drive[i]->actuation.velocity;
            params->drive[i].actuation.current = params->rt_drive[i]->actuation.current;
        }
        else
        {
            // Is the requested mode valid?
            if(params->rt_drive[i]->actuation.mode == DIFFERENTIAL_DRIVE_COMMAND_MODE_TORQUE ||
                params->rt_drive[i]->actuation.mode == DIFFERENTIAL_DRIVE_COMMAND_MODE_VELOCITY)
                *coord_state->configuration_request = true;
            params->drive[i].actuation.velocity.right_wheel = 0;
            params->drive[i].actuation.velocity.left_wheel = 0;
            params->drive[i].actuation.current.right_wheel = 0;
            params->drive[i].actuation.current.left_wheel = 0;
        }
        mtx_unlock(coord_state->drive_lock[i]);
    }

    // Copy desired kelo wheels commands (provided by an external activity via shared memory)
    // lock mutex
    for (int i = 0; i < kelo_soem_state->nb_smart_wheel_detected; i++)
    {
        kelo_soem_state->cmd_data_smart_wheel[i]->command1 |= (COM1_ENABLE1 |
            COM1_ENABLE2);
        // @NOTE: Kelo support team told us command2 shall not be set for now.
        // kelo_soem_state->cmd_data_smart_wheel[i]->command2 = COM1_ENABLE1 |
        //    COM1_ENABLE2 | params->kelo_input.command_mode;

        switch (params->drive[i].actuation.mode)
        {
        case DIFFERENTIAL_DRIVE_COMMAND_MODE_VELOCITY:
            kelo_soem_state->cmd_data_smart_wheel[i]->setpoint1 = 
                params->drive[i].actuation.velocity.right_wheel;
            kelo_soem_state->cmd_data_smart_wheel[i]->setpoint2 = 
                params->drive[i].actuation.velocity.left_wheel;
            break;
        case DIFFERENTIAL_DRIVE_COMMAND_MODE_TORQUE:
	    kelo_soem_state->cmd_data_smart_wheel[i]->setpoint1 = 
                params->drive[i].actuation.current.right_wheel;
            kelo_soem_state->cmd_data_smart_wheel[i]->setpoint2 = 
                params->drive[i].actuation.current.left_wheel;
            break;
        default:
            kelo_soem_state->cmd_data_smart_wheel[i]->setpoint1 = 0;
            kelo_soem_state->cmd_data_smart_wheel[i]->setpoint2 = 0;
            break;
        }
    }

    // Read/write kelo command via ethercat
    read_write_ethercat(&continuous_state->kelo_soem_state);

    for (int i = 0; i < kelo_soem_state->nb_smart_wheel_detected; i++)
    {
        txpdo1_to_differential_drive_sensor(
            continuous_state->kelo_soem_state.sensor_data_smart_wheel[i],
            &params->drive[i].sensor);
    }

    // Sharing kelo sensor data with other activities via shared memory
    for (int i = 0; i < kelo_soem_state->nb_smart_wheel_detected; i++)
    {
        mtx_lock(coord_state->drive_lock[i]);
        params->rt_drive[i]->sensor = params->drive[i].sensor;
        mtx_unlock(coord_state->drive_lock[i]);
    }

    // Logging to lttng
    lttng_ust_tracepoint(logging, drive_sensor_trace, 
        params->sensor_chunk_id, &params->drive[0].sensor);
    params->sensor_chunk_id++;
}

void mobile_platform_drive_kelo_activity_running_coordinate(activity_t *activity)
{
    mobile_platform_drive_kelo_activity_coordination_state_t *coord_state = 
        (mobile_platform_drive_kelo_activity_coordination_state_t *)activity->state.coordination_state;
    // Coordinating with other activities
    if (*coord_state->configuration_request){
        activity->lcsm.state = CAPABILITY_CONFIGURATION;
        *coord_state->configuration_request = false;
    }
    if (*coord_state->deleting_request){
        activity->state.lcsm_protocol = DEINITIALISATION;
        activity->lcsm.state = RESOURCE_CONFIGURATION;
    }
    update_super_state_lcsm_flags(&activity->state.lcsm_flags, activity->lcsm.state);
}

void mobile_platform_drive_kelo_activity_running_configure(activity_t *activity)
{
    if (activity->lcsm.state != RUNNING)
    {
        // Update schedule
        add_schedule_to_eventloop(&activity->schedule_table, "activity_config");
        remove_schedule_from_eventloop(&activity->schedule_table, "running");
    }
}

void mobile_platform_drive_kelo_activity_running(activity_t *activity)
{
    mobile_platform_drive_kelo_activity_running_communicate(activity);
    mobile_platform_drive_kelo_activity_running_coordinate(activity);
    mobile_platform_drive_kelo_activity_running_configure(activity);
}

// SCHEDULER
void mobile_platform_drive_kelo_activity_register_schedules(activity_t *activity)
{
    schedule_t schedule_config = {.number_of_functions = 0};
    register_function(&schedule_config, 
        (function_ptr_t)mobile_platform_drive_kelo_activity_config,
        activity, "activity_config");
    register_schedule(&activity->schedule_table, schedule_config, "activity_config");

    schedule_t schedule_creation = {.number_of_functions = 0};
    register_function(&schedule_creation, 
        (function_ptr_t)mobile_platform_drive_kelo_activity_creation,
        activity, "creation");
    register_schedule(&activity->schedule_table, schedule_creation, "creation");

    schedule_t schedule_resource_configuration = {.number_of_functions = 0};
    register_function(&schedule_resource_configuration, 
        (function_ptr_t)mobile_platform_drive_kelo_activity_resource_configuration,
        activity, "resource_configuration");
    register_schedule(&activity->schedule_table, schedule_resource_configuration, "resource_configuration");

    schedule_t schedule_capability_configuration = {.number_of_functions = 0};
    register_function(&schedule_capability_configuration, 
        (function_ptr_t)mobile_platform_drive_kelo_activity_capability_configuration,
        activity, "capability_configuration");
    register_schedule(&activity->schedule_table, schedule_capability_configuration, "capability_configuration");

    schedule_t schedule_running = {.number_of_functions = 0};
    register_function(&schedule_running, 
        (function_ptr_t)mobile_platform_drive_kelo_activity_running,
        activity, "running");
    register_schedule(&activity->schedule_table, schedule_running, "running");
}

void mobile_platform_drive_kelo_activity_create_lcsm(activity_t *activity, const char *name_activity)
{
    activity->conf.params = malloc(sizeof(mobile_platform_drive_kelo_activity_params_t));
    activity->state.computational_state.continuous = malloc(sizeof(mobile_platform_drive_kelo_activity_continuous_state_t));
    activity->state.computational_state.discrete = malloc(sizeof(mobile_platform_drive_kelo_activity_discrete_state_t));
    activity->state.coordination_state = malloc(sizeof(mobile_platform_drive_kelo_activity_coordination_state_t));
}

void mobile_platform_drive_kelo_activity_configure_lcsm(activity_t *activity, variable_table_t *table)
{
    mobile_platform_drive_kelo_activity_coordination_state_t *coord_state = 
        (mobile_platform_drive_kelo_activity_coordination_state_t *)activity->state.coordination_state;
    mobile_platform_drive_kelo_activity_params_t *params = 
        (mobile_platform_drive_kelo_activity_params_t *)activity->conf.params;

    configure_lcsm_activity(activity, table);
    // Select the inital state of LCSM for this activity
    activity->lcsm.state = CREATION;
    activity->state.lcsm_protocol = INITIALISATION;

    // Set pointers to NULL
    coord_state->execution_request = NULL;
    coord_state->deleting_request = NULL;
    coord_state->configuration_request = NULL;

    for(int i=0; i<MAX_NUMBER_SMARTWHEELS; i++)
    {
        params->rt_drive[i] = NULL;
        coord_state->drive_lock[i] = NULL;
    }

    // Schedule table (adding config() for the first eventloop iteration)
    mobile_platform_drive_kelo_activity_register_schedules(activity);
    add_schedule_to_eventloop(&activity->schedule_table, "activity_config");
}

void mobile_platform_drive_kelo_activity_destroy_lcsm(activity_t *activity)
{
    destroy_activity(activity);
}

const activity_lcsm_t mobile_platform_drive_kelo_activity_lcsm = {
    .create = mobile_platform_drive_kelo_activity_create_lcsm,
    .configure = mobile_platform_drive_kelo_activity_configure_lcsm,
    .destroy = mobile_platform_drive_kelo_activity_destroy_lcsm,
};

void configure_mobile_platform_drive_kelo_activity_from_file(const char *file_path,
    mobile_platform_drive_kelo_activity_params_t *params, int *status){

    int number_of_params = 0; // Amount of parameters to be read, set by user
    param_array_t param_array[16];

    param_array[number_of_params++] = (param_array_t){"kelo/port", 
        (params->kelo_soem_params.ifname), PARAM_TYPE_CHAR};
    param_array[number_of_params++] = (param_array_t){"kelo/max_number_of_connection_attempts", 
        &(params->max_number_of_connection_attempts), PARAM_TYPE_INT};
    param_array[number_of_params++] = (param_array_t){"kelo/min_time_between_connection_attempts", 
        &(params->min_time_between_connection_attempts), PARAM_TYPE_FLOAT};

    param_array[number_of_params++] = (param_array_t){"kelo/config/params/pivot_offset", 
        &(params->drive->config.params.pivot_offset), PARAM_TYPE_DOUBLE};
    param_array[number_of_params++] = (param_array_t){"kelo/config/params/wheel_radius", 
        &(params->drive->config.params.wheel_radius), PARAM_TYPE_DOUBLE};
    param_array[number_of_params++] = (param_array_t){"kelo/config/params/wheel_track", 
        &(params->drive->config.params.wheel_track), PARAM_TYPE_DOUBLE};

    param_array[number_of_params++] = (param_array_t){"kelo/config/limits/max_wheel_velocity", 
        &(params->drive->config.limits.max_wheel_velocity), PARAM_TYPE_DOUBLE};
    param_array[number_of_params++] = (param_array_t){"kelo/config/limits/max_wheel_acceleration", 
        &(params->drive->config.limits.max_wheel_acceleration), PARAM_TYPE_DOUBLE};

    char path_to_id_name[MAX_NUMBER_SMARTWHEELS][30];
    char path_to_id_sw_serial_number[MAX_NUMBER_SMARTWHEELS][50];
    for(int i = 0; i < MAX_NUMBER_SMARTWHEELS; i++){
        path_to_id_name[i][0] = '\0';
        sprintf(path_to_id_name[i], "kelo/id/wheel_%d/name", i);
        param_array[number_of_params++] = (param_array_t){path_to_id_name[i], 
            &(params->id[i].name), PARAM_TYPE_CHAR};
        path_to_id_sw_serial_number[i][0] = '\0';
        sprintf(path_to_id_sw_serial_number[i], "kelo/id/wheel_%d/sw_serial_number", i);
        param_array[number_of_params++] = (param_array_t){path_to_id_sw_serial_number[i], 
            &(params->id[i].sw_serial_number), PARAM_TYPE_INT};
    }
    // generic reader function
    int config_status_activity;

    read_from_input_file(file_path, param_array, number_of_params, &config_status_activity);

    // Verification
    if (config_status_activity == CONFIGURATION_FROM_FILE_SUCCEEDED)
    {
        *status = CONFIGURATION_FROM_FILE_SUCCEEDED;
    }
    else
    {
        *status = CONFIGURATION_FROM_FILE_FAILED;
    }
}
