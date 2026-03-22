#ifndef SUDDEN_JUMP_DETECTION_H
#define SUDDEN_JUMP_DETECTION_H
#define WINDOW_SIZE 500
#include <stdint.h>

typedef struct sud_jump_cfg_s
{
    uint8_t win_len;
    float thr;
} sud_jump_cfg_t;

typedef struct sud_jump_res_s
{
    float* diff;
    uint16_t* loc;
    float* fit;
    uint8_t jump_found;
    uint8_t jump_direction;
} sud_jump_res_t;
typedef struct {
    float* data;
    uint16_t length;
} Dataset;


typedef struct {
  double left_wheel_velocity[WINDOW_SIZE];
  double right_wheel_velocity[WINDOW_SIZE];
  double timestamp[WINDOW_SIZE]; 
  int counter;
} SensorDataArrays;


uint8_t sudden_jump_detection(sud_jump_cfg_t* config, sud_jump_res_t* result, double* data, uint16_t data_len);
uint8_t collision_detection_function(SensorDataArrays sensor_data, uint16_t win_len, float thr);
void store_sensor_data(double time,double left_velocity,double right_velocity, SensorDataArrays *sensor_data_arrays );  


#endif /* SUDDEN_JUMP_DETECTION_H */