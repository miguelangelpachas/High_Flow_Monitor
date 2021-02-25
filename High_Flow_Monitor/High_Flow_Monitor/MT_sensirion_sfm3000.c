



/* File inclusion */
#include <avr/io.h>
#include <stdint.h>
#include "project_defines.h"
#include <util/delay.h>
#include "global.h"
#include "twi_master.h"
#include "MT_sensirion_sfm3000.h"


/* Global variable definition */
static float proximal_flow_slpm, tidal_volume_ml;
static uint8_t sensor_read_data_buffer[3];
static uint8_t sensor_write_data_buffer[2];


/* Function definition */

void Proximal_Flow_Sensor_Initialize(void){
	
	ret_code_t i2c_error;

	/* Wait for the sensor to boot */
	_delay_ms(200);

	/* Command for resetting the sensor */
	Proximal_Flow_Sensor_Send_Command(SENSIRION_SOFTWARE_RESET);

	/* Wait for the sensor to start */
	_delay_ms(100);

	/* Command for continuous reading mode */
	Proximal_Flow_Sensor_Send_Command(SENSIRION_START_FLOW_MEASUREMENT);

	/* Wait for the sensor to be ready */
	_delay_ms(100);

	/* Read the flow measurement (binary value) */
	i2c_error = tw_master_receive(PROXIMAL_FLOW_SENSOR_SLAVE_ADDRESS, sensor_read_data_buffer, 3);
	
	/*  Wait for the first sample to be completed and discard it (it is not reliable) */
	_delay_ms(1);

}


void Proximal_Flow_Sensor_Read(void){

	uint16_t proximal_flow_bin;
	ret_code_t i2c_error = 0;
	uint8_t data_corrupted = 0;

	/* Read flow sensor binary value */
	sensor_read_data_buffer[0] = 0;
	sensor_read_data_buffer[1] = 0;
	sensor_read_data_buffer[2] = 0;

	/* Send command for continuous reading mode*/
	i2c_error = Proximal_Flow_Sensor_Send_Command(SENSIRION_START_FLOW_MEASUREMENT);
	/* Read the flow sensor */
	i2c_error = tw_master_receive(PROXIMAL_FLOW_SENSOR_SLAVE_ADDRESS, sensor_read_data_buffer, 3);
	
	/* Check if the proximal flow provided the flow measurement */
	if(i2c_error){

		/* If not, resend command for continuous reading mode */
		i2c_error = Proximal_Flow_Sensor_Send_Command(SENSIRION_START_FLOW_MEASUREMENT);

		/* Check if the sensor is working properly */
		if(i2c_error){
				system_flags |= (1 << FLOW_SENSOR_ERROR_FLAG);	
		}

	}else{

		system_flags &= ~(1 << FLOW_SENSOR_ERROR_FLAG);

		/* Extract the data from the i2c data frames  */
		proximal_flow_bin = (((uint16_t)sensor_read_data_buffer[0] << 8) | (uint16_t)sensor_read_data_buffer[1]);
		data_corrupted = Proximal_Flow_Sensor_CRC_Check((uint8_t *)sensor_read_data_buffer, 3, 0);

		/* Check if the received data is reliable */
		if(!data_corrupted){
			/* Calculate flow value in Standard Liter Per Minute (SLPM) */
			proximal_flow_slpm = ((float)proximal_flow_bin - PROXIMAL_FLOW_SENSOR_OFFSET_SLPM) / PROXIMAL_FLOW_SENSOR_SCALE_FACTOR;
		}

	}
	
}


/*
void Proximal_Flow_Sensor_Calculate_Volume(float flow_signal_slpm){

	static uint8_t integral_count = 0;
	static float f_x[3] = {0.0, 0.0, 0.0};
	float delta_volume_ml;
	
	f_x[2] = f_x[1];
	f_x[1] = f_x[0];
	f_x[0] = flow_signal_slpm;
	
	integral_count++;
	
	if(integral_count >= 2){
		delta_volume_ml = ((float)PROXIMAL_FLOW_SENSOR_READ_PERIOD_MS / 180.0) * (f_x[2] + (4 * f_x[1]) + f_x[0]);
		tidal_volume_ml += delta_volume_ml;
		integral_count = 0;
	}
	
}
*/

float Proximal_Flow_Sensor_Get_Flow(void){
	return proximal_flow_slpm;
}


/*
float Proximal_Flow_Sensor_Get_Volume(void){
	return tidal_volume_ml;
}
*/

/*
void Proximal_Flow_Sensor_Reset_Volume(void){
	tidal_volume_ml = 0.0;
}
*/


uint8_t Proximal_Flow_Sensor_CRC_Check(uint8_t data[], uint8_t n_bytes, uint8_t checksum){

	uint8_t crc = 0;
	uint8_t byte_counter;
	uint8_t checksum_error;

	//calculates 8-Bit checksum with given polynomial
	for (byte_counter = 0; byte_counter < n_bytes; ++byte_counter) {
		crc ^= (data[byte_counter]);
		for (uint8_t bit = 8; bit > 0; --bit) {
			if (crc & 0x80){
				crc = (crc << 1) ^ PROXIMAL_FLOW_SENSOR_CRC_POLYNOM;
				}else{
				crc = (crc << 1);
			}
		}
	}

	if (crc != checksum){
		checksum_error = 1;
		}else{
		checksum_error = 0;
	}
	return checksum_error;
	
}


uint8_t Proximal_Flow_Sensor_Send_Command(uint16_t command){

	ret_code_t i2c_error;

	/* Decompose the command */
	sensor_write_data_buffer[0] = (command >> 8) & 0xFF;
	sensor_write_data_buffer[1] = command & 0xFF;
	i2c_error = tw_master_transmit(PROXIMAL_FLOW_SENSOR_SLAVE_ADDRESS, sensor_write_data_buffer, 2, 0);

	return i2c_error;

}


uint16_t Proximal_Flow_Sensor_Get_Scale_Factor(void){

	ret_code_t i2c_error;
	uint16_t scale_factor;

	sensor_read_data_buffer[0] = 0;
	sensor_read_data_buffer[1] = 0;

	Proximal_Flow_Sensor_Send_Command(SENSIRION_READ_SCALE_FACTOR);

	/* Read sensor scale factor */
	i2c_error = tw_master_receive(PROXIMAL_FLOW_SENSOR_SLAVE_ADDRESS, sensor_read_data_buffer, 2);

	scale_factor = ((uint16_t)sensor_read_data_buffer[0] << 8) | (uint16_t)sensor_read_data_buffer[1];

	return scale_factor;
}



uint16_t Proximal_Flow_Sensor_Get_Offset(void){

	ret_code_t i2c_error;
	uint16_t offset;

	sensor_read_data_buffer[0] = 0;
	sensor_read_data_buffer[1] = 0;

	Proximal_Flow_Sensor_Send_Command(SENSIRION_READ_OFFSET);

	/* Read sensor scale factor */
	i2c_error = tw_master_receive(PROXIMAL_FLOW_SENSOR_SLAVE_ADDRESS, sensor_read_data_buffer, 2);

	offset = ((uint16_t)sensor_read_data_buffer[0] << 8) | (uint16_t)sensor_read_data_buffer[1];

	return offset;

}
