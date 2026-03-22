#ifndef SENSOR_DATA_STRUCTURE_LIDAR_H
#define SENSOR_DATA_STRUCTURE_LIDAR_H

#include <sensor_data_structure/range_scan.h>
#include <sensor_data_structure/range_sensor.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct lidar_s{
    range_sensor_t *range_sensor;
    range_scan_t *range_scan;
}lidar_t;

#ifdef __cplusplus
}
#endif

#endif