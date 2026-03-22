
#ifndef SENSOR_DATA_STRUCTURE_DIFFERENTIAL_DRIVE_SENSOR_H
#define SENSOR_DATA_STRUCTURE_DIFFERENTIAL_DRIVE_SENSOR_H

#include <time.h>

#include <sensor_data_structure/encoder.h>
#include <sensor_data_structure/accelerometer.h>
#include <sensor_data_structure/gyroscope.h>
#include <data_structure/time/timestamp.h>

#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct differential_drive_sensor_s
{
    timestamp_t timestamp;    
    accelerometer_t accelerometer;
    gyroscope_t gyroscope;
    struct
    {
        encoder_t left_wheel, right_wheel, pivot;
    }encoder;
    struct 
    {
        float left_wheel, right_wheel;
    }temperature;
    struct
    {
        float left_wheel, right_wheel;
    }voltage_pwm, voltage_u, voltage_v, voltage_w;
    float voltage_bus;
    struct
    {
        float left_wheel, right_wheel;
    }current_d, current_q, current_u, current_v, current_w;
}differential_drive_sensor_t;

#ifdef __cplusplus
}
#endif

#endif