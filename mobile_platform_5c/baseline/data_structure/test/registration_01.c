#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>
#include <threads.h>
#include <math.h>

#include <data_structure/registration/hash_table_variable.h>

#include <sensor_data_structure/encoder.h>
#include <sensor_data_structure/range_scan.h>

#define NUMBER_OF_RANGE_BEAMS 5
#define LIDAR_FREQUENCY_IN_HZ 10
#define ENCODER_FREQUENCY_IN_HZ 100

static void sigint_handler(int sig); 
int mock_lidar_activity(void *args);
int mock_encoder_activity(void *args);
int mock_communication_activity(void *args);
void print_lidar(range_scan_t *range_scan, mtx_t *mutex);
void print_encoder(encoder_t *encoder, mtx_t *mutex);

bool deinitialisation_request = false;

typedef struct {
    char dataname[50];
    char datatype[50];
    variable_table_t *table;
}mock_activity_t;

int main(int argc, char** argv)
{
    // Create a table for data structure objects
    variable_table_t table;
    create_variable_table(&table);
    mock_activity_t lidar_activity = {.dataname="lidar", .table=&table};    // datatype: range_scan_t
    mock_activity_t left_encoder_activity = {.dataname="left", .table=&table};  // datatype: encoder_t
    mock_activity_t right_encoder_activity = {.dataname="right", .table=&table};
    mock_activity_t communication_activity = {.table=&table};
    
    // Creating threads
    thrd_t lidar_thrd, left_encoder_thrd, right_encoder_thrd;
    thrd_t communication_thrd;
    thrd_create(&lidar_thrd, mock_lidar_activity, &lidar_activity);
    thrd_create(&left_encoder_thrd, mock_encoder_activity, &left_encoder_activity);
    thrd_create(&right_encoder_thrd, mock_encoder_activity, &right_encoder_activity);
    thrd_create(&communication_thrd, mock_communication_activity, &communication_activity);

    int res;
    thrd_join(lidar_thrd, &res);
    thrd_join(left_encoder_thrd, &res);
    thrd_join(right_encoder_thrd, &res);
    thrd_join(communication_thrd, &res);
    return 0;
}

int mock_communication_activity(void *args)
{
    mock_activity_t *activity = (mock_activity_t *) args;
    variable_table_t *table = activity->table;
    
    hash_result_t hash_result;
    char hash_result_string[30];
    // Running
    while(!deinitialisation_request)
    {
        sleep(1);
        printf("-------------------------------------\n");
        // Get pointer
        variable_entry_t *entry=NULL;
        semantic_id_t sid;
        
        //************************************************//
        //*     This will come from the high-level       *//
        //************************************************//
        memset(&sid, 0, sizeof(semantic_id_t));
        printf("data name: ");
        scanf("%s", sid.model);
        printf("data type: ");
        scanf("%s", sid.meta_model);

        hash_result = get_variable_entry(&table->root, &sid, &entry);
        hash_result_to_string(hash_result, hash_result_string);
        printf("[communication_activity][GET] %s %s: %s \n", 
            sid.model, sid.meta_model, hash_result_string); 
        if(hash_result==HASH_ENTRY_FOUND)
        {
            printf("Void pointer %p pointing to data type %s, lock with %p\n", 
            entry->pointer, sid.meta_model, &entry->mutex); 
            //************************************************//
            //*  How to choose the right printing function?  *//
            //*  who says print, says trace :)               *//
            //************************************************//
            //************************************************//
            // Here goes a terrible way to do it...
            // It will not scale well.
            //************************************************//
            if (strcmp("range_scan_t", sid.meta_model)==0)
            {
                print_lidar((range_scan_t*) entry->pointer, &entry->mutex);
            }
            else if (strcmp("encoder_t", sid.meta_model)==0)
            {
                print_encoder((encoder_t*) entry->pointer, &entry->mutex);
            }
        }
    }
}

void print_lidar(range_scan_t *range_scan, mtx_t *mutex)
{
    printf("range_scan: ");
    mtx_lock(mutex);
    for(int i=0; i<range_scan->nb_measurements; i++)
    {
        printf("%.3f ", range_scan->measurements[i]);
    }
    mtx_unlock(mutex);
    printf("\n");
}

void print_encoder(encoder_t *encoder, mtx_t *mutex)
{
    mtx_lock(mutex);
    printf("encoder pulses: %d\n", encoder->pulses);
    printf("encoder pos: %f\n", encoder->position);
    printf("encoder vel: %f\n", encoder->velocity);
    mtx_unlock(mutex);
}

int mock_lidar_activity(void *args)
{
    mock_activity_t *activity = (mock_activity_t *) args;
    variable_table_t *table = activity->table;
    hash_result_t hash_result;
    char hash_result_string[30];

    // Registration
    semantic_id_t data_sid;
    memset(&data_sid, 0, sizeof(data_sid));
    strcpy(data_sid.model, activity->dataname);
    variable_registration_args_t encoder_reg_args = {.data=data_sid};
    REGISTER_VARIABLE_IN_TABLE(table, encoder_reg_args, range_scan_t, hash_result);
    hash_result_to_string(hash_result, hash_result_string);
    printf("[mock_lidar_activity][REGISTER] %s range_scan_t: %s \n", 
       activity->dataname, hash_result_string);

    // Get pointer
    range_scan_t *range_scan = NULL;
    mtx_t *mutex = NULL;
    GET_VARIABLE_IN_TABLE(table, encoder_reg_args, range_scan_t, range_scan, mutex, hash_result);
    hash_result_to_string(hash_result, hash_result_string);
    printf("[mock_lidar_activity][GET] %s range_scan_t: %s at void pointer %p, lock with %p\n", 
       activity->dataname, hash_result_string, range_scan, mutex);
    // Check if succeded
    if(hash_result == HASH_ENTRY_NOT_FOUND)
    {
        return 0;
    }
    
    // // // Memory allocation for range scan (resource configure)
    range_scan->nb_measurements = NUMBER_OF_RANGE_BEAMS;
    range_scan->measurements = (double *) malloc(sizeof(double)*NUMBER_OF_RANGE_BEAMS);
    range_scan->angles = (double *) malloc(sizeof(double)*NUMBER_OF_RANGE_BEAMS);

    // Faking lidar data (compute)
    int sleeping_time = (int) ( (1.0/LIDAR_FREQUENCY_IN_HZ) *1000*1000);
    while(!deinitialisation_request)
    {
        usleep(sleeping_time); // time in microseconds
        mtx_lock(mutex);
        for(int i=0; i<range_scan->nb_measurements; i++)
        {
            range_scan->measurements[i] = ((double) rand())/RAND_MAX;
            range_scan->angles[i] = (M_PI/2*i)/(NUMBER_OF_RANGE_BEAMS-1) - M_PI/2 ;
        }
        clock_gettime(CLOCK_MONOTONIC_RAW, &range_scan->timestamp);
        mtx_unlock(mutex);
    }
}

int mock_encoder_activity(void *args)
{
    mock_activity_t *activity = (mock_activity_t *) args;
    variable_table_t *table = activity->table;
    hash_result_t hash_result;
    char hash_result_string[30];

    // Register and get variable
    semantic_id_t data_sid;
    strcpy(data_sid.model, activity->dataname); 
    variable_registration_args_t encoder_reg_args = {.data=data_sid};

    encoder_t *encoder = NULL;
    mtx_t *mutex = NULL;
    REGISTER_AND_GET_VARIABLE_IN_TABLE(table, encoder_reg_args, encoder_t, encoder, mutex, hash_result);
    printf("[mock_lidar_activity][REGISTER/GET] %s encoder_t: %s at void pointer %p, lock with %p\n", 
       activity->dataname, hash_result_string, encoder, mutex);
    
    // Check if succeded
    if(hash_result != HASH_ENTRY_ADDED)
    {
        return 0;
    }
    
    // Faking encoder data (compute)
    int sleeping_time = (int) ( (1.0/ENCODER_FREQUENCY_IN_HZ) *1000*1000);
    while(!deinitialisation_request)
    {
        usleep(sleeping_time); // time in microseconds
        mtx_lock(mutex);
        encoder->velocity = .1;
        encoder->position += encoder->velocity/ENCODER_FREQUENCY_IN_HZ;
        encoder->pulses += 1;
        clock_gettime(CLOCK_MONOTONIC_RAW, &encoder->timestamp);
        mtx_unlock(mutex);
    }
}