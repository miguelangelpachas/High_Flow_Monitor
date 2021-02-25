
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include "project_defines.h"
#include "global.h"
#include "MT_adc.h"
#include "fio2_sensor.h"


/* Data stored in the EEPROM */
float EEMEM ee_calib_line_slope = FIO2_CALIB_LINE_SLOPE_BY_DEFAULT;
float EEMEM	ee_calib_line_offset = FIO2_CALIBE_LINE_OFFSET_BY_DEFAULT;

/* Variable definition */
float calib_line_slope = FIO2_CALIB_LINE_SLOPE_BY_DEFAULT;
float calib_line_offset = FIO2_CALIBE_LINE_OFFSET_BY_DEFAULT;
static float fio2_percent = 0;

void FiO2_Sensor_Read(void){

	float sensor_volt_out;
	float sensor_volt_offset;
	float sensor_final_voltage;
	uint16_t o2_sensor_bin_value;
	uint16_t o2_sensor_offset_bin_value;
	
	/* Select the corresponding ADC channel for O2 sensor*/
	ADC_Seleccionar_Canal(ADC_CHANNEL_OXYGEN_SENSOR_VALUE);
	/* Start a new conversion */
	ADCSRA |= (1 << ADSC);
	while(!(ADCSRA & (1 << ADIF))){
		/* No hacer nada hasta que la conversión finalice */
	}
	ADCSRA |= (1 << ADIF);
	
	o2_sensor_bin_value = ADC;
	
	/* Select the corresponding ADC channel for O2 sensor offset*/
	ADC_Seleccionar_Canal(ADC_CHANNEL_OXYGEN_SENSOR_OFFSET);
	/* Start a new conversion */
	ADCSRA |= (1 << ADSC);
	while(!(ADCSRA & (1 << ADIF))){
		/* No hacer nada hasta que la conversión finalice */
	}
	ADCSRA |= (1 << ADIF);
	
	o2_sensor_offset_bin_value = ADC;

	/* Measure the voltage that comes from the sensor amplifier */
	sensor_volt_out = (((float)o2_sensor_bin_value) * ADC_REFERENCE_VOLTAGE) / 1024.0;
	/* Measure the offset voltage */
	sensor_volt_offset = (((float)o2_sensor_offset_bin_value) * ADC_REFERENCE_VOLTAGE) / 1024.0;
	/* Calculate the voltage  that comes from the oxygen sensor */
	sensor_final_voltage = (sensor_volt_out - sensor_volt_offset) / FIO2_AMP_GAIN;
	/* Determine whether the sensor is connected or not */
	if(sensor_final_voltage < FIO2_SENSOR_ERROR_THRESHOLD_V){
		cli();
		system_flags |= (1 << FIO2_SENSOR_ERROR_FLAG);
		sei();
	}else{
		cli();
		system_flags &= ~(1 << FIO2_SENSOR_ERROR_FLAG);
		sei();		
	}

	//fio2_percent = 100.0 * (sensor_final_voltage - FIO2_SENSOR_MIN_OUT_V) / (FIO2_SENSOR_MAX_OUT_V  - FIO2_SENSOR_MIN_OUT_V);
	fio2_percent = (calib_line_slope * sensor_final_voltage) + calib_line_offset;
	
	if(fio2_percent > 100.0){
		fio2_percent = 100.0;
	}else if(fio2_percent < 0.0){
		fio2_percent = 0.0;
	}else{
		// Does nothing
	}
}



float Get_FiO2_Value(void){

	return fio2_percent;

}


void Save_FiO2_Calib_Line_Slope_In_EEPROM(float new_slope){
	eeprom_busy_wait();
	eeprom_update_float(&ee_calib_line_slope, new_slope);
}


void Save_FiO2_Calib_Line_Offset_In_EEPROM(float new_offset){
	eeprom_busy_wait();
	eeprom_update_float(&ee_calib_line_offset, new_offset);
}


void Load_FiO2_Calib_Line_Slope_From_EEPROM(void){
	eeprom_busy_wait();
	calib_line_slope = eeprom_read_float(&ee_calib_line_slope);
}


void Load_FiO2_Calib_Line_Offset_From_EEPROM(void){
	eeprom_busy_wait();
	calib_line_offset = eeprom_read_float(&ee_calib_line_offset);
}