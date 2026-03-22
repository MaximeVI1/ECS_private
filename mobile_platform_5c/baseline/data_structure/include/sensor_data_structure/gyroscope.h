#ifndef SENSOR_DATA_STRUCTURE_GYROSCOPE_H
#define SENSOR_DATA_STRUCTURE_GYROSCOPE_H

#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct gyroscope_s{
    struct timespec timestamp;    ///< As defined in time.h
    double x, y, z;
}gyroscope_t;

#ifdef __cplusplus
}
#endif

#endif