#ifndef SENSOR_DATA_STRUCTURE_ACCELEROMETER_H
#define SENSOR_DATA_STRUCTURE_ACCELEROMETER_H

#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct accelerometer_s
{
    struct timespec timestamp;    
    double x, y, z;
}accelerometer_t;

#ifdef __cplusplus
}
#endif

#endif