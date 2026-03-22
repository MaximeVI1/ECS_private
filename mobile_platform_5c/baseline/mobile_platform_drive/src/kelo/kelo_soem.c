/* ----------------------------------------------------------------------------
 * Project Title,
 * ROB @ KU Leuven, Leuven, Belgium
 * See LICENSE for the license information
 * -------------------------------------------------------------------------- */

/**
 * @file kelo_soem.c
 * @date October 12, 2021
 **/

#include <mobile_platform_drive/kelo/kelo_soem.h>

#include <stdio.h>

#define EC_TIMEOUTMON 500

#define SW_SERIAL_NUMBER_INDEX 0x8020
#define SW_SERIAL_NUMBER_SUBINDEX 0x2c

bool enabled_ecat_monitor_activity = true;
boolean inOP = 1;
int expectedWKC;
boolean needlf;
volatile int wkc;
uint8 currentgroup = 0;
int smartWheelStatus[MAX_NUMBER_SMARTWHEELS];
char IOmap[4096]; // 4kB

bool initialise_ethercat_master(kelo_soem_state_t *state, kelo_soem_params_t *params){

    //printf("Starting EtherCAT master\n");
    // initialise SOEM, bind socket to ifname (single NIC mode) 
    if (ec_init(params->ifname))
    {
        // find and auto-config slaves
        // Enumerate and init all slaves.
        if ( ec_config_init(FALSE) > 0 ) {
            // Map all PDOs from slaves to IOmap with Outputs/Inputs
            ec_config_map(&state->IOmap);
            // Locate DC slaves, measure propagation delays.
            ec_configdc();
            // wait for all slaves to reach SAFE_OP state 
            ec_statecheck(0, EC_STATE_SAFE_OP, EC_TIMEOUTSTATE * 4);   // STATE SAFE operation 
            // read individual slave state and store in ec_slave[]
            ec_readstate();
            // Store datagram pointers of each KELO wheel
            // cmd_data_smart_wheel_t_[nROPODSmartWheel]  :  rxpdo wheel commands
            // sensor_data_smart_wheel_t_[nROPODSmartWheel] :  txpdo sensor readings
            int psize;
            for(int cnt = 1; cnt <= ec_slavecount ; cnt++)
            {
                // Assign each known device the corresponding data memory pointer
                switch (ec_slave[cnt].eep_id) {

                    case SMARTWHEEL_VENDOR_ID:
                        // Smart wheel has been found
                        // Here the convention is that output refers to data transmitted
                        // to the EC device and input to data received from the EC device
                        state->cmd_data_smart_wheel[state->nb_smart_wheel_detected] = (rxpdo1_t*) ec_slave[cnt].outputs;
                        state->sensor_data_smart_wheel[state->nb_smart_wheel_detected] = (txpdo1_t*) ec_slave[cnt].inputs;
                        // Get sw_serial_number, index = 0x8020, subindex = 0x2c
                        psize = sizeof(uint32_t);
                        ec_SDOread( (uint16) cnt, (uint16) SW_SERIAL_NUMBER_INDEX, 
                            (uint8) SW_SERIAL_NUMBER_SUBINDEX, FALSE,
                            &psize, &state->sw_serial_number[state->nb_smart_wheel_detected], EC_TIMEOUTSTATE);

                        state->smartWheelStatus[state->nb_smart_wheel_detected] = 1;
                        state->nb_smart_wheel_detected += 1;
                        break;
                    default:
                        // Unregistered slave detected 
                        break;
                }
            }

            // Expected Work counter (Request operational state for all slaves) 
            // compute work counter, determines read/write type of datagram
            state->expected_work_counter = (ec_group[0].outputsWKC * 2) + ec_group[0].inputsWKC;
            // wait for all slaves to reach OPERATIONAL state 
            ec_slave[0].state = EC_STATE_OPERATIONAL;
            // send one valid process data to make outputs in slaves happy
            ec_send_processdata();
            ec_receive_processdata(EC_TIMEOUTRET);
            // request OP state for all slaves 
            ec_writestate(0);
            int chk = 40;
            // wait for all slaves to reach OP state 
            do {
                ec_send_processdata();
                ec_receive_processdata(EC_TIMEOUTRET);
                ec_statecheck(0, EC_STATE_OPERATIONAL, 50000);  // STATE OPERATIONAL
            } while (chk-- && (ec_slave[0].state != EC_STATE_OPERATIONAL));

            if (ec_slave[0].state == EC_STATE_OPERATIONAL) {
                // Operational state reached for all slaves
                state->in_operational_state = true;
                return true;
            } else {
                // Not all slaves reached operational state
                ec_readstate();
                for (int i = 1; i <= ec_slavecount; i++) {
                    if (ec_slave[i].state != EC_STATE_OPERATIONAL) {
                        ;//printf("Slave %d: State = 0x%2.2x, StatusCode = 0x%4.4x : %s\n",
                        //         i, ec_slave[i].state, ec_slave[i].ALstatuscode, ec_ALstatuscode2string(ec_slave[i].ALstatuscode));
                    }
                }
            }
            // Request init state for all slaves
            ec_slave[0].state = EC_STATE_INIT;
            // request INIT state for all slaves 
            ec_writestate(0);
        }
        // stop SOEM, close socket 
        ec_close();
    }

    return false;
}

void shutdown_ethercat_master(kelo_soem_state_t *state, kelo_soem_params_t *params){
        // Shutting down correctly
        // Request init state for all slaves
        ec_slave[0].state = EC_STATE_INIT; 
        ec_writestate(0);
        // stop SOEM, close socket 
        ec_close();
}

void read_write_ethercat(kelo_soem_state_t *state) {
    state->has_new_measurement = false;
    // Transmit processdata to slaves.
    // Uses LRW, or LRD/LWR if LRW is not allowed (blockLRW).
    // Both the input and output processdata are transmitted.
    // The inputs are gathered with the receive processdata function.
    // In contrast to the base LRW function this function is non-blocking.
    ec_send_processdata();
    // Receive processdata from slaves.
    state->work_counter = ec_receive_processdata(EC_TIMEOUTRET);

    if (state->work_counter >= state->expected_work_counter ) {
        // Writing to circular ring buffer the sensor data from the smart wheel
        state->has_new_measurement = true;
    }
}

void txpdo1_to_differential_drive_sensor(txpdo1_t *sensor_data_smart_wheel, 
    differential_drive_sensor_t *drive_sensor){

    drive_sensor->encoder.right_wheel.position = (double) sensor_data_smart_wheel->encoder_1;
    drive_sensor->encoder.right_wheel.velocity = (double) sensor_data_smart_wheel->velocity_1;
    drive_sensor->encoder.left_wheel.position = (double) sensor_data_smart_wheel->encoder_2;
    drive_sensor->encoder.left_wheel.velocity = (double) sensor_data_smart_wheel->velocity_2;
    drive_sensor->encoder.pivot.position = (double) sensor_data_smart_wheel->encoder_pivot;
    drive_sensor->encoder.pivot.velocity = (double) sensor_data_smart_wheel->velocity_pivot;
    drive_sensor->accelerometer.x = (double) sensor_data_smart_wheel->accel_x;
    drive_sensor->accelerometer.y = (double) sensor_data_smart_wheel->accel_y;
    drive_sensor->accelerometer.z = (double) sensor_data_smart_wheel->accel_z;
    drive_sensor->gyroscope.x = (double) sensor_data_smart_wheel->gyro_x;
    drive_sensor->gyroscope.y = (double) sensor_data_smart_wheel->gyro_y;
    drive_sensor->gyroscope.z = (double) sensor_data_smart_wheel->gyro_z;
    drive_sensor->temperature.right_wheel = (float) sensor_data_smart_wheel->temperature_1;
    drive_sensor->temperature.left_wheel = (float) sensor_data_smart_wheel->temperature_2;
    drive_sensor->voltage_bus = (float) sensor_data_smart_wheel->voltage_bus;
    // Voltage
    drive_sensor->voltage_pwm.right_wheel = (float) sensor_data_smart_wheel->voltage_1;
    drive_sensor->voltage_pwm.left_wheel = (float) sensor_data_smart_wheel->voltage_2;
    drive_sensor->voltage_u.right_wheel = (float) sensor_data_smart_wheel->voltage_1_u;
    drive_sensor->voltage_u.left_wheel = (float) sensor_data_smart_wheel->voltage_2_u;
    drive_sensor->voltage_v.right_wheel = (float) sensor_data_smart_wheel->voltage_1_v;
    drive_sensor->voltage_v.left_wheel = (float) sensor_data_smart_wheel->voltage_2_v;
    drive_sensor->voltage_w.right_wheel = (float) sensor_data_smart_wheel->voltage_1_w;
    drive_sensor->voltage_w.left_wheel = (float) sensor_data_smart_wheel->voltage_2_w;
    // Current
    drive_sensor->current_d.right_wheel = (float) sensor_data_smart_wheel->current_1_d;
    drive_sensor->current_d.left_wheel = (float) sensor_data_smart_wheel->current_2_d;
    drive_sensor->current_q.right_wheel = (float) sensor_data_smart_wheel->current_1_q;
    drive_sensor->current_q.left_wheel = (float) sensor_data_smart_wheel->current_2_q;
    drive_sensor->current_u.right_wheel = (float) sensor_data_smart_wheel->current_1_u;
    drive_sensor->current_u.left_wheel = (float) sensor_data_smart_wheel->current_2_u;
    drive_sensor->current_v.right_wheel = (float) sensor_data_smart_wheel->current_1_v;
    drive_sensor->current_v.left_wheel = (float) sensor_data_smart_wheel->current_2_v;
    drive_sensor->current_w.right_wheel = (float) sensor_data_smart_wheel->current_1_w;
    drive_sensor->current_w.left_wheel = (float) sensor_data_smart_wheel->current_2_w;
    // time
    drive_sensor->timestamp.seconds = sensor_data_smart_wheel->sensor_ts/1000000000;
    drive_sensor->timestamp.nanoseconds = sensor_data_smart_wheel->sensor_ts % 1000000000;
}
