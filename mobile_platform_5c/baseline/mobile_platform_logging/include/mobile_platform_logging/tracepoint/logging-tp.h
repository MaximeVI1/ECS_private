#undef LTTNG_UST_TRACEPOINT_PROVIDER
#define LTTNG_UST_TRACEPOINT_PROVIDER logging

#undef LTTNG_UST_TRACEPOINT_INCLUDE
#define LTTNG_UST_TRACEPOINT_INCLUDE "mobile_platform_logging/tracepoint/logging-tp.h"

#if !defined(_LOGGING_TP_H) || defined(LTTNG_UST_TRACEPOINT_HEADER_MULTI_READ)
#define _LOGGING_TP_H

#include <lttng/tracepoint.h>

// Data structure headers
#include <drive_data_structure/differential_drive.h>
#include <actuation_data_structure/differential_drive_actuation.h>
#include <sensor_data_structure/differential_drive_sensor.h>
#include <sensor_data_structure/range_scan.h>


/*
 * Use LTTNG_UST_TRACEPOINT_EVENT(), LTTNG_UST_TRACEPOINT_EVENT_CLASS(),
 * LTTNG_UST_TRACEPOINT_EVENT_INSTANCE(), and
 * LTTNG_UST_TRACEPOINT_LOGLEVEL() here.
 */

LTTNG_UST_TRACEPOINT_EVENT(
    /* Tracepoint provider name */
    logging,

    /* Tracepoint name */
    range_scan_trace,

    /* Input arguments */
    LTTNG_UST_TP_ARGS(
        range_scan_float_t *, range_scan
    ),

    /* Output event fields */
    LTTNG_UST_TP_FIELDS(
        lttng_ust_field_float(long, timestamp, range_scan->timestamp.tv_nsec)
        lttng_ust_field_integer(int, nb_measurements,range_scan->nb_measurements)
        lttng_ust_field_sequence(float, measurements,range_scan->measurements,unsigned int,range_scan->nb_measurements)
        lttng_ust_field_sequence(float, angles,range_scan->angles,unsigned int,range_scan->nb_measurements)
    )
)


LTTNG_UST_TRACEPOINT_EVENT(
    /* Tracepoint provider name */
    logging,

    /* Tracepoint name */
    drive_actuation_trace,

    /* Input arguments */
    LTTNG_UST_TP_ARGS(
        int, id,
        differential_drive_actuation_t *, drive_actuation
    ),

    /* Output event fields */
    LTTNG_UST_TP_FIELDS(
        lttng_ust_field_integer(int,id, id)
        lttng_ust_field_float(float,timestamp, drive_actuation->timestamp.tv_nsec)
        lttng_ust_field_integer(int,mode, drive_actuation->mode)
        lttng_ust_field_float(float, vel_left_wheel, (float)drive_actuation->velocity.left_wheel)
        lttng_ust_field_float(float, vel_right_wheel, (float)drive_actuation->velocity.right_wheel)
        lttng_ust_field_float(float, current_left_wheel, (float)drive_actuation->current.left_wheel)
        lttng_ust_field_float(float, current_right_wheel, (float)drive_actuation->current.right_wheel)
    )
)


LTTNG_UST_TRACEPOINT_EVENT(
    /* Tracepoint provider name */
    logging,

    /* Tracepoint name */
    drive_sensor_trace,

    /* Input arguments */
    LTTNG_UST_TP_ARGS(
        int, id,
        differential_drive_sensor_t *, drive_sensor
    ),

    /* Output event fields */
    LTTNG_UST_TP_FIELDS(
        lttng_ust_field_integer(int,id, id)
        lttng_ust_field_float(float,timestamp_sec, drive_sensor->timestamp.seconds)
        lttng_ust_field_float(float,timestamp_nanosec, drive_sensor->timestamp.nanoseconds)

        lttng_ust_field_float(float, acc_x, (float)drive_sensor->accelerometer.x)
        lttng_ust_field_float(float, acc_y, (float)drive_sensor->accelerometer.y)
        lttng_ust_field_float(float, acc_z, (float)drive_sensor->accelerometer.z) 

        lttng_ust_field_float(float, gyroscope_x, (float)drive_sensor->gyroscope.x)
        lttng_ust_field_float(float, gyroscope_y, (float)drive_sensor->gyroscope.y)
        lttng_ust_field_float(float, gyroscope_z, (float)drive_sensor->gyroscope.z)  
    
        lttng_ust_field_float(float, encoder_pos_left, (float)drive_sensor->encoder.left_wheel.position)
        lttng_ust_field_float(float, encoder_vel_left, (float)drive_sensor->encoder.left_wheel.velocity)
        lttng_ust_field_integer(int, encoder_pulses_left, drive_sensor->encoder.left_wheel.pulses)

        lttng_ust_field_float(float, encoder_pos_right, (float)drive_sensor->encoder.right_wheel.position)
        lttng_ust_field_float(float, encoder_vel_right, (float)drive_sensor->encoder.right_wheel.velocity)
        lttng_ust_field_integer(int, encoder_pulses_right, drive_sensor->encoder.right_wheel.pulses)

        lttng_ust_field_float(float, encoder_pos_pivot, (float)drive_sensor->encoder.pivot.position)
        lttng_ust_field_float(float, encoder_vel_pivot, (float)drive_sensor->encoder.pivot.velocity)
        lttng_ust_field_integer(int, encoder_pulses_pivot, drive_sensor->encoder.pivot.pulses)
        
        lttng_ust_field_float(float, temp_left_wheel, (float)drive_sensor->temperature.left_wheel)
        lttng_ust_field_float(float, temp_right_wheel, (float)drive_sensor->temperature.right_wheel)

        lttng_ust_field_float(float, volt_pwm_left_wheel, (float)drive_sensor->voltage_pwm.left_wheel)
        lttng_ust_field_float(float, volt_pwm_right_wheel, (float)drive_sensor->voltage_pwm.right_wheel)
        lttng_ust_field_float(float, volt_u_left_wheel, (float)drive_sensor->voltage_u.left_wheel)
        lttng_ust_field_float(float, volt_u_right_wheel, (float)drive_sensor->voltage_u.right_wheel)
        lttng_ust_field_float(float, volt_v_left_wheel, (float)drive_sensor->voltage_v.left_wheel)
        lttng_ust_field_float(float, volt_v_right_wheel, (float)drive_sensor->voltage_v.right_wheel)
        lttng_ust_field_float(float, volt_w_left_wheel, (float)drive_sensor->voltage_w.left_wheel)
        lttng_ust_field_float(float, volt_w_right_wheel, (float)drive_sensor->voltage_w.right_wheel)
        lttng_ust_field_float(float, volt_bus, (float)drive_sensor->voltage_bus)

        lttng_ust_field_float(float, curr_d_left_wheel, (float)drive_sensor->current_d.left_wheel)
        lttng_ust_field_float(float, curr_d_right_wheel, (float)drive_sensor->current_d.right_wheel)
        lttng_ust_field_float(float, curr_q_left_wheel, (float)drive_sensor->current_q.left_wheel)
        lttng_ust_field_float(float, curr_q_right_wheel, (float)drive_sensor->current_q.right_wheel)
        lttng_ust_field_float(float, curr_u_left_wheel, (float)drive_sensor->current_u.left_wheel)
        lttng_ust_field_float(float, curr_u_right_wheel, (float)drive_sensor->current_u.right_wheel)
        lttng_ust_field_float(float, curr_v_left_wheel, (float)drive_sensor->current_v.left_wheel)
        lttng_ust_field_float(float, curr_v_right_wheel, (float)drive_sensor->current_v.right_wheel)
        lttng_ust_field_float(float, curr_w_left_wheel, (float)drive_sensor->current_w.left_wheel)
        lttng_ust_field_float(float, curr_w_right_wheel, (float)drive_sensor->current_w.right_wheel)       
        )
)

#endif /* _LOGGING_TP_H */

#include <lttng/tracepoint-event.h>