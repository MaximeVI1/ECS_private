#include <mobile_platform_proprioception/delta_tricycle.h>

void odometry_from_encoders_delta_tricycle(
    delta_tricycle_t *platform, differential_drive_t *drive,
    double sampling_time_ms, odometry2d_t *odometry)
{
    // Pose of the active wheel w.r.t. to the center of rear axle of the tricycle
    double drive_to_plaform_tx = platform->wheel.pose.x;
    double drive_to_plaform_ty = platform->wheel.pose.y;
    double drive_orientation_wrt_platform = platform->wheel.pose.yaw;

    // Avoiding singularities..
    if (fabs(drive_to_plaform_tx) < 1e-3 ||
        drive->config.params.wheel_track < 1e-3){ 
        return;
    }

    // Steering angle of the driving vehicle w.r.t the forward direction of the triycle
    double steering_angle = drive_orientation_wrt_platform + drive->sensor.encoder.pivot.position;
    double cos_steering_angle = cos(steering_angle);
    double sin_steering_angle = sin(steering_angle);

    // The intermediate steps below are for keeping the code clean. There
    // is no direct interest in the drive's velocity and steering rate. 
    // [Intermediate step] Current forward velocity at the drive frame    
    double current_drive_velocity = .5*drive->config.params.wheel_radius*(
        drive->sensor.encoder.left_wheel.velocity - 
        drive->sensor.encoder.right_wheel.velocity);
    // [Intermediate step]     
    double current_drive_steering_rate = drive->config.params.wheel_radius*(
        -drive->sensor.encoder.left_wheel.velocity - 
        drive->sensor.encoder.right_wheel.velocity)/
        drive->config.params.wheel_track;

    // Estimation of the current linear and angular velocity of the platform 
    // from sensor data
    odometry->velocity.vx = current_drive_velocity*(cos_steering_angle + 
        (drive_to_plaform_ty/drive_to_plaform_tx)*sin_steering_angle) +
        current_drive_steering_rate*drive->config.params.pivot_offset*(-sin_steering_angle + 
        (drive_to_plaform_ty/drive_to_plaform_tx)*cos_steering_angle);
    odometry->velocity.vy = 0;
    odometry->velocity.w = current_drive_velocity*(sin_steering_angle/drive_to_plaform_tx) +
        current_drive_steering_rate*drive->config.params.pivot_offset*(cos_steering_angle/
            drive_to_plaform_tx);
    
    // Integrate velocity (forward euler integration) using result in previous step
    double dt = sampling_time_ms/1000.0;
    odometry->pose.x += odometry->velocity.vx*cos(odometry->pose.yaw)*dt;
    odometry->pose.y += odometry->velocity.vx*sin(odometry->pose.yaw)*dt;
    odometry->pose.yaw = wrap_to_pi(odometry->pose.yaw + odometry->velocity.w*dt); 
}

