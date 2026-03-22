// Standard libraries
#include <signal.h>
#include <pthread.h>
#include <string.h>

// Five-c
#include <five_c/activity/activity.h>
#include <five_c/thread/thread.h>

// Load from configuration file
#include <read_file/read_file.h>

// Activites
#include <navigation/activity/navigation_activity.h>
#include <mobile_platform_drive/activity/kelo_drive_activity.h>
// #include <lidar/activity/hokuyo_activity.h>
#include <mobile_platform_proprioception/activity/activity.h>
// #include <mobile_platform_control/activity/activity.h>

// Data structures
#include <platform_data_structure/delta_tricycle.h>
#include <drive_data_structure/differential_drive.h>
#include <geometry_data_structure/pose2d.h>

// Logging
#include <mobile_platform_logging/tracepoint/logging-tp.h>


enum COMMAND_LINE_ARGUMENTS_STATUS {INVALID_COMMAND_LINE_ARGUMENTS, 
    VALID_COMMAND_LINE_ARGUMENTS}; 

typedef struct
{
    // Activities' flags
    bool configuration_request_for_drive;
    // Thread cycle time
    struct
    {
        int drive, lidar, control, navigation;
    }thread_cycle_time_in_ms;
    // configuration files
    struct
    {
        char drive[250], lidar[250];
        char proprioception[250], control[250];
        char navigation[250];
    }configuration_file;
}application_data_t;

enum COMMAND_LINE_ARGUMENTS_STATUS arguments_from_command_line(int argc, 
    char **argv);

void initalise_application_data_data_from_json_file(const char *file_path, 
    application_data_t *application_data, int *status);