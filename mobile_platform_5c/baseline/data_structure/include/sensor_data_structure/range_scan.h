#ifndef SENSOR_DATA_STRUCTURE_RANGE_SCAN_H
#define SENSOR_DATA_STRUCTURE_RANGE_SCAN_H

#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

// Adding this here for now
typedef struct range_scan_float_s{
    struct timespec timestamp;    ///< As defined in time.h
    float *angles;  ///< angle[i] is the direction of the i-th sensor beam (radians)
    float *measurements;   ///< measurement[i] is the distance to the object that reflected the i-th sensor beam (meters) 
    int nb_measurements;   ///< number of measurements in the current scan reading
}range_scan_float_t;

typedef struct range_scan_s{
    struct timespec timestamp;    ///< As defined in time.h
    double *angles;  ///< angle[i] is the direction of the i-th sensor beam (radians)
    double *measurements;   ///< measurement[i] is the distance to the object that reflected the i-th sensor beam (meters) 
    int nb_measurements;   ///< number of measurements in the current scan reading
}range_scan_t;

#ifdef __cplusplus
}
#endif

#endif