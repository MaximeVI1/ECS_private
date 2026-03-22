// Standard libraries
#include "app.h"

// 5C data structures
#include <data_structure/registration/hash_table_variable.h>

bool deleting_request = false;
bool execution_request = true;

static void sigint_handler(int sig);

int main(int argc, char** argv) 
{
	signal(SIGINT, sigint_handler);
    
    // Validate nubmer of arguments via command-line
    if (arguments_from_command_line(argc, argv) == INVALID_COMMAND_LINE_ARGUMENTS){
        return EXIT_FAILURE;
    }

    // Create the variable table shared between the activities
    variable_table_t variable_table;
    create_variable_table(&variable_table);

    // Initialise world model from configuration file
    int initialisation_succeded;
    application_data_t application_data;
    initalise_application_data_data_from_json_file(argv[1], &application_data, &initialisation_succeded);
    
    // Instatiate activites
    activity_t drive_activity;
    activity_t proprioception_activity;
    activity_t navigation_activity;

    // Create and configure LCSM of activities
    mobile_platform_drive_kelo_activity_lcsm.create(&drive_activity, "drive_activity");
    mobile_platform_drive_kelo_activity_lcsm.configure(&drive_activity, &variable_table);

    mobile_platform_proprioception_activity_lcsm.create(&proprioception_activity, "proprioception_activity");
    mobile_platform_proprioception_activity_lcsm.configure(&proprioception_activity, &variable_table);

    navigation_activity_lcsm.create(&navigation_activity, "navigation_activity");
    navigation_activity_lcsm.configure(&navigation_activity, &variable_table);

    // Instatiate  threads
    thread_t thread_drive, thread_navigation;
    create_thread(&thread_drive, "drive", application_data.thread_cycle_time_in_ms.drive);  
    create_thread(&thread_navigation, "navigation", application_data.thread_cycle_time_in_ms.navigation);

    // Register activities in thread
    register_activity(&thread_drive, &drive_activity, NULL);
    register_activity(&thread_drive, &proprioception_activity, NULL);
    register_activity(&thread_navigation, &navigation_activity, NULL);

    // Shared memory
    mobile_platform_drive_kelo_activity_params_t *drive_activity_params =
        (mobile_platform_drive_kelo_activity_params_t *) drive_activity.conf.params;
    mobile_platform_drive_kelo_activity_coordination_state_t *drive_activity_coord_state =
        (mobile_platform_drive_kelo_activity_coordination_state_t *) drive_activity.state.coordination_state;

    drive_activity_coord_state->execution_request = &execution_request;
    drive_activity_coord_state->deleting_request = &deleting_request;
    drive_activity_coord_state->configuration_request = &application_data.configuration_request_for_drive;

    mobile_platform_proprioception_activity_params_t *proprioception_activity_params =
        (mobile_platform_proprioception_activity_params_t *) proprioception_activity.conf.params;
    mobile_platform_proprioception_activity_coordination_state_t *proprioception_activity_coord_state =
        (mobile_platform_proprioception_activity_coordination_state_t *) proprioception_activity.state.coordination_state;

    proprioception_activity_coord_state->execution_request = &execution_request;
    proprioception_activity_coord_state->deleting_request = &deleting_request;

    navigation_activity_params_t *navigation_activity_params =
        (navigation_activity_params_t *) navigation_activity.conf.params;
    navigation_activity_coordination_state_t *navigation_activity_coord_state =
        (navigation_activity_coordination_state_t *) navigation_activity.state.coordination_state;

    navigation_activity_coord_state->execution_request = &execution_request;
    navigation_activity_coord_state->deleting_request = &deleting_request;

    // Copying configuration files
    strcpy(drive_activity_params->configuration_file, application_data.configuration_file.drive);
    strcpy(proprioception_activity_params->configuration_file, application_data.configuration_file.proprioception);
    strcpy(navigation_activity_params->configuration_file, application_data.configuration_file.navigation);
 
     // Create and execute POSIX threads
    pthread_t pthread_drive; 
    pthread_t pthread_navigation;
    pthread_create( &pthread_drive, NULL, do_thread_loop, ((void*) &thread_drive));
    pthread_create( &pthread_navigation, NULL, do_thread_loop, ((void*) &thread_navigation));
 
     // Wait for thread to finish
    pthread_join(pthread_drive, NULL);
    pthread_join(pthread_navigation, NULL);
 
    // Freeing memory
    mobile_platform_drive_kelo_activity_lcsm.destroy(&drive_activity);
    mobile_platform_proprioception_activity_lcsm.destroy(&proprioception_activity);           
    navigation_activity_lcsm.destroy(&navigation_activity);     
}

static void sigint_handler(int sig)
{
    printf("\nDeinitialising activities\n");
	execution_request = false;
	deleting_request = true;
}

enum COMMAND_LINE_ARGUMENTS_STATUS arguments_from_command_line(int argc, char **argv)
{
    if (argc != 2) {
        printf("The correct usage is..\n");
        printf("<executable> <path_to_configuration_file> \n\n"); 
        return INVALID_COMMAND_LINE_ARGUMENTS;
    }
    else {
        return VALID_COMMAND_LINE_ARGUMENTS;
    }
}


void initalise_application_data_data_from_json_file(const char *file_path, 
    application_data_t *application_data, int *status)
{
     param_array_t param_array[] = 
    {
        {.param_name="thread_cycle_time_in_ms/drive", 
            .param_pointer=&application_data->thread_cycle_time_in_ms.drive, 
            .param_type=PARAM_TYPE_INT},
        {.param_name="thread_cycle_time_in_ms/lidar", 
            .param_pointer=&application_data->thread_cycle_time_in_ms.lidar, 
            .param_type=PARAM_TYPE_INT},
        {.param_name="thread_cycle_time_in_ms/navigation", 
            .param_pointer=&application_data->thread_cycle_time_in_ms.navigation, 
            .param_type=PARAM_TYPE_INT},
        {.param_name="configuration_file/drive", 
            .param_pointer=application_data->configuration_file.drive, 
            .param_type=PARAM_TYPE_CHAR},
        {.param_name="configuration_file/lidar", 
            .param_pointer=application_data->configuration_file.lidar, 
            .param_type=PARAM_TYPE_CHAR},
        {.param_name="configuration_file/proprioception", 
            .param_pointer=application_data->configuration_file.proprioception, 
            .param_type=PARAM_TYPE_CHAR},
        {.param_name="configuration_file/navigation", 
            .param_pointer=application_data->configuration_file.navigation, 
            .param_type=PARAM_TYPE_CHAR}
    };

    int number_of_parameters =  sizeof(param_array)/sizeof(param_array_t);
    read_from_input_file(file_path, param_array, number_of_parameters, status);
}
