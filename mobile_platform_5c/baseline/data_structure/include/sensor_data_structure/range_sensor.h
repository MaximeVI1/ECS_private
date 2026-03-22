#ifndef SENSOR_DATA_STRUCTURE_RANGE_SENSOR_H
#define SENSOR_DATA_STRUCTURE_RANGE_SENSOR_H

#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct range_sensor_s{
    struct timespec timestamp;    ///< As defined in time.h
    double min_angle, max_angle;    ///< angular limits of the sensor [radians]
    double min_distance, max_distance;    ///< range distance limits [meters]
    double accuracy;    ///< accuracy of the sensor [%/meter]
    double angular_resolution;    ///< angular distance between two beams
    int nb_measurements;    ///< number of beams per scan reading
}range_sensor_t;

#ifdef __cplusplus
}
#endif

#endif
