// processdata structures and global user defined types

#ifndef SMART_WHEEL_API
#define SMART_WHEEL_API

// Needed for the PACKED structures
#include "ethercat.h"

#define STAT1_ENABLED1          0x0001
#define STAT1_ENABLED2          0x0002
#define STAT1_ENC_1_OK			0x0004
#define STAT1_ENC_2_OK			0x0008
#define STAT1_ENC_PIVOT_OK		0x0010
#define STAT1_UNDERVOLTAGE		0x0020
#define STAT1_OVERVOLTAGE		0x0040
#define STAT1_OVERCURRENT_1		0x0080
#define STAT1_OVERCURRENT_2		0x0100
#define STAT1_OVERTEMP_1		0x0200
#define STAT1_OVERTEMP_2		0x0400

#define STAT2_UNUSED			0x0000

#define SMARTWHEEL_VENDOR_ID            0x2001001

typedef struct PACKED{
  uint16_t		status1;			// Status bits as defined in STAT1_
  uint16_t		status2;			// Status bits as defined in STAT2_
  uint64_t		sensor_ts;			// EtherCAT timestamp (ns) on sensor acquisition
  uint64_t		setpoint_ts;		// EtherCAT timestamp (ns) of last setpoint data
  float			encoder_1;			// encoder 1 value in rad (no wrapping at 2PI)
  float			velocity_1;			// encoder 1 velocity in rad/s
  float			current_1_d;		// motor 1 current direct in amp
  float			current_1_q;		// motor 1 current quadrature in amp
  float			current_1_u;		// motor 1 current phase U in amp
  float			current_1_v;		// motor 1 current phase V in amp
  float			current_1_w;		// motor 1 current phase W in amp
  float			voltage_1;			// motor 1 voltage from pwm in volts
  float			voltage_1_u;		// motor 1 voltage from phase U in volts
  float			voltage_1_v;		// motor 1 voltage from phase V	in volts
  float			voltage_1_w;		// motor 1 voltage from phase W in volts
  float			temperature_1;		// motor 1 estimated temperature in K
  float			encoder_2;			// encoder 2 value in rad (no wrapping at 2PI)
  float			velocity_2;         // encoder 2 velocity in rad/s
  float			current_2_d;		// motor 2 current direct in amp
  float			current_2_q;		// motor 2 current quadrature in amp
  float			current_2_u;		// motor 2 current phase U in amp
  float			current_2_v;		// motor 2 current phase V in amp
  float			current_2_w;		// motor 2 current phase W in amp
  float			voltage_2;			// motor 2 voltage from pwm in volts
  float			voltage_2_u;		// motor 2 voltage from phase U in volts
  float			voltage_2_v;		// motor 2 voltage from phase V	in volts
  float			voltage_2_w;		// motor 2 voltage from phase W in volts
  float			temperature_2;		// motor 2 estimated temperature in K
  float			encoder_pivot;		// encoder pivot value in rad (wrapping at -PI and +PI)
  float			velocity_pivot;		// encoder pivot velocity in rad/s
  float			voltage_bus;		// bus voltage in volts
  uint64_t		imu_ts;				// EtherCAT timestamp (ns) of IMU sensor acquisition
  float			accel_x;			// IMU accelerometer X-axis in m/s2
  float			accel_y;			// IMU accelerometer Y-axis in m/s2
  float			accel_z;			// IMU accelerometer Z-axis in m/s2
  float			gyro_x;				// IMU gyro X-axis in rad/s
  float			gyro_y;				// IMU gyro Y-axis in rad/s
  float			gyro_z;				// IMU gyro Z-axis in rad/s
  float			temperature_imu;	// IMU temperature in K
  float			pressure;			// barometric pressure in Pa absolute
}txpdo1_t;

/* SMARTWHEEL SETPOINT MODES
/*
/* Mode TORQUE
/*   Setpoint 1		= Current in Amp for motor 1
/*   Setpoint 2		= Current in Amp for motor 2
/*   Upper limit 1 	= Most positive velocity (rad/s) allowed for motor 1
/*   Lower limit 1  = Most negative velocity (rad/s) allowed for motor 1
/*   Upper limit 2 	= Most positive velocity (rad/s) allowed for motor 2
/*   Lower limit 2  = Most negative velocity (rad/s) allowed for motor 2
/*
/* Mode DTORQUE
/*   Setpoint 1		= Common current in Amp
/*   Setpoint 2		= Differential current in Amp
/*   Upper limit 1 	= Most positive velocity (rad/s) allowed for linear motion
/*   Lower limit 1  = Most negative velocity (rad/s) allowed for linear motion
/*   Upper limit 2 	= Most positive velocity (rad/s) allowed for pivot motion
/*   Lower limit 2  = Most negative velocity (rad/s) allowed for pivot motion
/*
/* Mode VELOCITY
/*   Setpoint 1		= Velocity in rad/s for motor 1
/*   Setpoint 2		= Velocity in rad/s for motor 2
/*   Upper limit 1 	= Most positive current (amp) allowed for motor 1
/*   Lower limit 1  = Most negative current (amp) allowed for motor 1
/*   Upper limit 2 	= Most positive current (amp) allowed for motor 2
/*   Lower limit 2  = Most negative current (amp) allowed for motor 2
/*
/* Mode DVELOCITY
/*   Setpoint 1		= Common velocity in rad/s
/*   Setpoint 2		= Differential velocity in rad/s
/*   Upper limit 1 	= Most positive current (amp) allowed for linear motion
/*   Lower limit 1  = Most negative current (amp) allowed for linear motion
/*   Upper limit 2 	= Most positive current (amp) allowed for pivot motion
/*   Lower limit 2  = Most negative current (amp) allowed for pivot motion
/*
*/

#define COM1_ENABLE1          0x0001
#define COM1_ENABLE2          0x0002
#define COM1_MODE_TORQUE	  (0x0 << 2)
#define COM1_MODE_DTORQUE	  (0x1 << 2)
#define COM1_MODE_VELOCITY	  (0x2 << 2)
#define COM1_MODE_DVELOCITY	  (0x3 << 2)
#define COM1_EMERGENCY1		  0x0010
#define COM1_EMERGENCY2		  0x0020
#define COM1_USE_TS			  0x8000

#define COM2_UNUSED			  0x0000

typedef struct PACKED{
  uint16_t      command1;			// Command bits as defined in COM1_
  uint16_t		command2;			// Command bits as defined in COM2_
  float			setpoint1;			// Setpoint 1
  float			setpoint2;			// Setpoint 2
  float			limit1_p;			// Upper limit 1
  float			limit1_n;			// Lower limit 1
  float			limit2_p;			// Upper limit 2
  float			limit2_n;			// Lower limit 2
  uint64_t		timestamp;			// EtherCAT timestamp (ns) setpoint execution
}rxpdo1_t;

typedef struct {
  float			wheeldistance;			// distance between wheels in m
  float			casterdistance;			// caster offset in m
  float			pivotoffset;			// pivot encoder offset in rad
  float         p_vc_pgain;				// pivot velocity control P gain
  float         p_vc_igain;				// pivot velocity control I gain
  float         p_vc_ilimit;			// pivot velocity control I limiter
  float			p_vc_ffgain;			// pivot velocity model feed forward gain
  float         m1resistance;			// motor 1 phase resistance in ohm
  float			m1inductance;			// motor 1 phase inductance in henry
  float         m1kv;					// motor 1 speed constant in rad/vs
  float			m1thermalr;				// motor 1 thermal resistance in K/W
  float			m1thermalt;				// motor 1 thermal time constant s
  float			m1diameter;				// motor 1 wheel diameter in m
  float         m1_cc_pgain;			// motor 1 current control P gain
  float         m1_cc_igain;			// motor 1 current control I gain
  float         m1_cc_ilimit;			// motor 1 current control I limiter
  float			m1_cc_ffgain;			// motor 1 current model feed forward gain
  float         m1_vc_pgain;			// motor 1 velocity control P gain
  float         m1_vc_igain;			// motor 1 velocity control I gain
  float         m1_vc_ilimit;			// motor 1 velocity control I limiter
  float			m1_vc_ffgain;			// motor 1 velocity model feed forward gain
  float			m1_enc_offset;			// motor 1 encoder offset in rad
  float			m1_enc_ppairs;			// motor 1 pole pairs, negative for phase reversal
  float         m2resistance;			// motor 2 phase resistance in ohm
  float			m2inductance;			// motor 2 phase inductance in henry
  float         m2kv;					// motor 2 speed constant in rad/vs
  float			m2thermalr;				// motor 2 thermal resistance in K/W
  float			m2thermalt;				// motor 2 thermal time constant s
  float			m2diameter;				// motor 2 wheel diameter in m
  float         m2_cc_pgain;			// motor 2 current control P gain
  float         m2_cc_igain;			// motor 2 current control I gain
  float         m2_cc_ilimit;			// motor 2 current control I limiter
  float			m2_cc_ffgain;			// motor 2 current model feed forward gain
  float         m2_vc_pgain;			// motor 2 velocity control P gain
  float         m2_vc_igain;			// motor 2 velocity control I gain
  float         m2_vc_ilimit;			// motor 2 velocity control I limiter
  float			m2_vc_ffgain;			// motor 2 velocity model feed forward gain
  float			m2_enc_offset;			// motor 2 encoder offset in rad
  float			m2_enc_ppairs;			// motor 2 pole pairs, negative for phase reversal
}param_t;

typedef struct {
  float			voltage_min;			// minimum allowed voltage in enabled state in V
  float			voltage_min_time;		// time constant for minimum voltage filter in s
  float			voltage_max;			// maximum allowed voltage in enabled state in V
  float			current_max;			// maximum allowed peak current in A
  float			drive_temp_max;			// maximum allowed drive temperature in K
  float			motor_temp_max;			// maximum allowed motor temperature in K
  float			motor_current_limit;	// motor current limit in A
  float			motor_speed_limit;		// motor speed limit in rad/s
  float			watchdog_time;			// OP mode watchdog time out in s
  float			distance_total;			// total travel distance in m
  float			pivot_total;			// total pivots in rad
}drvparam_t;

typedef struct PACKED{
  float			p_encoder_gain;			// pivot encoder gain in rad/bit
  float			bv_offset;				// bus voltage offset in V
  float			bv_gain;				// bus voltage sensor gain in V/bit
  float			m1_cu_offset;			// motor 1 current phase U offset in A
  float			m1_cu_gain;				// motor 1 current phase U gain in A/bit
  float			m1_cv_offset;			// motor 1 current phase V offset in A
  float			m1_cv_gain;				// motor 1 current phase V gain in A/bit
  float			m1_cw_offset;			// motor 1 current phase W offset in A
  float			m1_cw_gain;				// motor 1 current phase W gain in A/bit
  float			m1_vu_offset;			// motor 1 voltage phase U offset in V
  float			m1_vu_gain;				// motor 1 voltage phase U gain in V/bit
  float			m1_vv_offset;			// motor 1 voltage phase U offset in V
  float			m1_vv_gain;				// motor 1 voltage phase U gain in V/bit
  float			m1_vw_offset;			// motor 1 voltage phase U offset in V
  float			m1_vw_gain;				// motor 1 voltage phase U gain in V/bit
  float			m1_encoder_gain;		// motor 1 encoder gain in rad/bit
  float			m2_cu_offset;			// motor 2 current phase U offset in A
  float			m2_cu_gain;				// motor 2 current phase U gain in A/bit
  float			m2_cv_offset;			// motor 2 current phase V offset in A
  float			m2_cv_gain;				// motor 2 current phase V gain in A/bit
  float			m2_cw_offset;			// motor 2 current phase W offset in A
  float			m2_cw_gain;				// motor 2 current phase W gain in A/bit
  float			m2_vu_offset;			// motor 2 voltage phase U offset in V
  float			m2_vu_gain;				// motor 2 voltage phase U gain in V/bit
  float			m2_vv_offset;			// motor 2 voltage phase U offset in V
  float			m2_vv_gain;				// motor 2 voltage phase U gain in V/bit
  float			m2_vw_offset;			// motor 2 voltage phase U offset in V
  float			m2_vw_gain;				// motor 2 voltage phase U gain in V/bit
  float			m2_encoder_gain;		// motor 2 encoder gain in rad/bit
  float			imu_acc_x_offset;		// IMU accelerometer X offset in m/s2
  float			imu_acc_x_gain;			// IMU accelerometer X gain in m/s2 / bit
  float			imu_acc_y_offset;		// IMU accelerometer Y offset in m/s2
  float			imu_acc_y_gain;			// IMU accelerometer Y gain in m/s2 / bit
  float			imu_acc_z_offset;		// IMU accelerometer Z offset in m/s2
  float			imu_acc_z_gain;			// IMU accelerometer Z gain in m/s2 / bit
  float			imu_gyro_x_offset;		// IMU gyro X offset in rad/s
  float			imu_gyro_x_gain;		// IMU gyro X gain in rad/s / bit
  float			imu_gyro_y_offset;		// IMU gyro Y offset in rad/s
  float			imu_gyro_y_gain;		// IMU gyro Y gain in rad/s / bit
  float			imu_gyro_z_offset;		// IMU gyro Z offset in rad/s
  float			imu_gyro_z_gain;		// IMU gyro Z gain in rad/s / bit
}sensparam_t;

#endif // #ifndef SMART_WHEEL_API
