#ifndef SENSOR_DATA_STRUCTURE_ENCODER_H
#define SENSOR_DATA_STRUCTURE_ENCODER_H

#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct encoder_s{
    struct timespec timestamp;    ///< As defined in time.h
    int pulses;  ///< number of pulses detected
    double position;  ///< encoder position in rad (do not wrap) 
    double velocity;  ///< encoder velocity in rad/s
}encoder_t;

#ifdef __cplusplus
}
#endif

#endif