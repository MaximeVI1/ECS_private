#ifndef MOBILE_PLATFORM_PROPRIOCEPTION_DELTA_TRICYCLE_H
#define MOBILE_PLATFORM_PROPRIOCEPTION_DELTA_TRICYCLE_H

#include <math.h>
#include <string.h>

#include <math_rob_c/numbers/numbers.h>
// 5C data structures
#include <drive_data_structure/differential_drive.h>
#include <platform_data_structure/delta_tricycle.h>
#include <navigation_data_structure/odometry2d.h>

#ifdef __cplusplus
extern "C" {
#endif

/*! \brief Computes odometry of a delta tricycle platform from 
 * the encoder measurements. 
 * 
 * This function computes the odometry (pose and velocity) of a
 * tricycle platform (<https://en.wikipedia.org/wiki/Tricycle>) 
 * with two fixed (passive) wheels and one active differential drive, 
 * e.g., a kelo wheel drive <https://www.kelo-robotics.com/technologies/>. 
 * The drive has to providet he data of three encoders: velocity of each 
 * wheel + position of the pivot. The reference of the position and 
 * velocity is attached to the center of the fixed wheels axle.
 * 
 * @param[in] platform config and state of the platform.
 * @param[in] wheel config and state of the active wheel.
 * @param[in] sampling_time_ms sampling time in miliseconds required 
 * for integrating the velocity.
 * @param[out] odometry estimated odometry of the platform
 */
void odometry_from_encoders_delta_tricycle(
    delta_tricycle_t *platform,
    differential_drive_t *wheel,
    double sampling_time_ms,
    odometry2d_t *odometry);


#ifdef __cplusplus
}
#endif

#endif