#ifndef LOGGING_DATA_STRUCTURE_SENSOR_LOG_ENTRY_H
#define LOGGING_DATA_STRUCTURE_SENSOR_LOG_ENTRY_H

#include <time.h>
#include <stdint.h>

#include <sensor_data_structure/differential_drive_sensor.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct sensor_log_entry_s{
    uint64_t id;
    differential_drive_sensor_t sensor;
} sensor_log_entry_t;

#ifdef __cplusplus
}
#endif

#endif