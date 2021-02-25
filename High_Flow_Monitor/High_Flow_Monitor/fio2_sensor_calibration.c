
/* File inclusion */
#include "project_defines.h"
#include <avr/interrupt.h>
#include <util/delay.h>
#include "global.h"
#include "MT_adc.h"
#include "mt_debounce.h"
#include "lcd_interface.h"
#include "fio2_sensor.h"
#include "fio2_sensor_calibration.h"
#include "MT_least_squares.h"


/* Variable definition */

float calibration_samples_x[N_CALIBRATION_TOTAL_SAMPLES];
float calibration_samples_y[N_CALIBRATION_TOTAL_SAMPLES];

float calibration_slope;
float calibration_offset;

uint8_t calib_upper_boundary_array[N_CALIBRATION_REFERENCE_POINTS] = {
																		CALIB_UPPER_BOUNDARY_01,
																		CALIB_UPPER_BOUNDARY_02,
																		CALIB_UPPER_BOUNDARY_03,
																		CALIB_UPPER_BOUNDARY_04,
																		CALIB_UPPER_BOUNDARY_05,
																		CALIB_UPPER_BOUNDARY_06,
																		CALIB_UPPER_BOUNDARY_07,
																		CALIB_UPPER_BOUNDARY_08
																	};

uint8_t calib_lower_boundary_array[N_CALIBRATION_REFERENCE_POINTS] = {
																		CALIB_LOWER_BOUNDARY_01,
																		CALIB_LOWER_BOUNDARY_02,
																		CALIB_LOWER_BOUNDARY_03,
																		CALIB_LOWER_BOUNDARY_04,
																		CALIB_LOWER_BOUNDARY_05,
																		CALIB_LOWER_BOUNDARY_06,
																		CALIB_LOWER_BOUNDARY_07,
																		CALIB_LOWER_BOUNDARY_08
																	};

/* Function definition */

void FiO2_Sensor_Calibration_Sequence(void){
	
	uint8_t calib_range_index = 0;
	uint8_t dummy = 0;
	
	/* Print the start calibration screen and keep it for 2 seconds*/
	LCD_Print_Calibration_Start_Screen();
	_delay_ms(2000);
	
	/* If the buttons have been pressed during the previous screen, ignore them */	
	dummy = G1_Get_Button_Press(1 << ALARM_ON_OFF_BUTTON);
	dummy = G1_Get_Button_Press(1 << SYSTEM_MENU_BUTTON);
	dummy = G1_Get_Button_Long(1 << ALARM_ON_OFF_BUTTON);
	dummy = G1_Get_Button_Long(1 << SYSTEM_MENU_BUTTON);
	dummy = G1_Get_Button_Release(1 << SYSTEM_ON_OFF_BUTTON);

	/* Sample voltage values (X-axis) and FiO2 values (Y-axis) for the whole range */
	for(calib_range_index = 0; calib_range_index < N_CALIBRATION_REFERENCE_POINTS; calib_range_index++){
		Calibration_Sampling_Sequence(calib_range_index);
	}
	
	/* Indicate that the CPU is processing the data */
	LCD_Print_Calibration_Processing_Screen();
	_delay_ms(1000);
	
	/* Check if the FiO2 is working properly, before starting the calculations */
	if(system_flags & (1 << FIO2_SENSOR_ERROR_FLAG)){
		cli();
		system_flags |= (1 << CALIBRATION_ERROR_FLAG);
		sei();
	}else{
		/* Use the samples to compute the slope and the offset of the calibration line */
		Calibration_Compute_Parameters();	
	}
	
	/* Indicate whether calibration was successful or not */
	if(system_flags & (1 << CALIBRATION_ERROR_FLAG)){
		
		if(system_flags & (1 << FIO2_SENSOR_ERROR_FLAG)){
			cli();
			system_flags &= ~(1 << FIO2_SENSOR_ERROR_FLAG);
			sei();
			LCD_Print_Calibration_Sensor_Error_Screen();
			_delay_ms(1000);
		}
		
		cli();
		system_flags &= ~(1 << CALIBRATION_ERROR_FLAG);
		sei();		
		
		LCD_Print_Calibration_Error_Screen();
			
	}else{
		/* Save the new parameters in EEPROM */
		Save_FiO2_Calib_Line_Slope_In_EEPROM(calibration_slope);
		Save_FiO2_Calib_Line_Offset_In_EEPROM(calibration_offset);
		LCD_Print_Calibration_Complete_Screen();
	}
	/* Wait for a moment */
	_delay_ms(2000);
	
	/* If the buttons have been pressed during the previous screen, ignore them */
	dummy = G1_Get_Button_Press(1 << ALARM_ON_OFF_BUTTON);
	dummy = G1_Get_Button_Press(1 << SYSTEM_MENU_BUTTON);
	dummy = G1_Get_Button_Long(1 << ALARM_ON_OFF_BUTTON);
	dummy = G1_Get_Button_Long(1 << SYSTEM_MENU_BUTTON);
	dummy = G1_Get_Button_Release(1 << SYSTEM_ON_OFF_BUTTON);	
}


void Calibration_Sample_Reference_Point(uint8_t index, uint8_t fio2_ref_val){
	
	uint32_t offset_index = 0;
	uint32_t sample_index = 0;

	float sensor_volt_out;
	float sensor_volt_offset;
	float sensor_final_voltage;
	uint16_t o2_sensor_bin_value;
	uint16_t o2_sensor_offset_bin_value;	
	
	offset_index = N_CALIBRATION_SAMPLES_PER_POINT * ((uint32_t)index);
	
	for(sample_index = 0; sample_index < N_CALIBRATION_SAMPLES_PER_POINT; sample_index++){

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

		/* Calcular el voltaje entregado por el amplificador del sensor*/
		sensor_volt_out = (((float)o2_sensor_bin_value) * ADC_REFERENCE_VOLTAGE) / 1024.0;
		/* Calcular el voltaje de offset */
		sensor_volt_offset = (((float)o2_sensor_offset_bin_value) * ADC_REFERENCE_VOLTAGE) / 1024.0;
	
		sensor_final_voltage = (sensor_volt_out - sensor_volt_offset) / FIO2_AMP_GAIN;
		
		if(sensor_final_voltage < FIO2_SENSOR_ERROR_THRESHOLD_V){
			cli();
			system_flags |= (1 << FIO2_SENSOR_ERROR_FLAG);
			sei();
		}
		
		/* Store samples and reference value in the respective arrays */
		calibration_samples_x[offset_index + sample_index] = sensor_final_voltage;
		calibration_samples_y[offset_index + sample_index] = (float)fio2_ref_val;
		
	}
	
}



void Calibration_Sampling_Sequence(uint8_t index){
	
	uint8_t upper_boundary, lower_boundary;
	uint8_t reference_value;
	uint8_t value_entered = 0;
	
	/* Initialize the reference value and determine its boundaries */
	upper_boundary = calib_upper_boundary_array[index];
	lower_boundary = calib_lower_boundary_array[index];
	reference_value = lower_boundary;
	
	/* Print the range header */
	LCD_Print_Calibration_Range_Header(index);
	LCD_Print_Calibration_Reference_Header();
	LCD_Print_Calibration_Reference_Value(reference_value);
	LCD_Print_Calibration_Reference_Units();
	
	while(value_entered == 0){
		
		/* Refresh the display */
		if(system_flags & (1 << DISPLAY_UPDATE_FLAG)){
			cli();
			system_flags &= ~(1 << DISPLAY_UPDATE_FLAG);
			sei();
			LCD_Print_Calibration_Reference_Value(reference_value);
		}		
		
		/* Update the reference value */
		if(G1_Get_Button_Short(1 << ALARM_ON_OFF_BUTTON)){
			reference_value++;
			if(reference_value > upper_boundary){
				reference_value = lower_boundary;
			}
		}
				
		/* Update the reference value */
		if(G1_Get_Button_Short(1 << SYSTEM_MENU_BUTTON)){
			value_entered = 1;
		}
		
		/* Prepare to shut down the system */
		if(G1_Get_Button_Release(1 << SYSTEM_ON_OFF_BUTTON)){
			cli();
			system_flags |= (1 << SYSTEM_SHUT_DOWN_FLAG);
			sei();
		}				
		
		/* Shut down the system */
		if(system_flags & (1 << SYSTEM_SHUT_DOWN_FLAG)){
			System_Shutdown_Message();	
		}		
		
	}
	
	Calibration_Sample_Reference_Point(index, reference_value);
}


void Calibration_Compute_Parameters(void){
	
	uint8_t division_by_zero = 0;

    Least_Squares_X_Sample_Vector_Fill_In((float *)calibration_samples_x, 0, N_CALIBRATION_TOTAL_SAMPLES);
    Least_Squares_Y_Sample_Vector_Fill_In(calibration_samples_y, 0, N_CALIBRATION_TOTAL_SAMPLES);
    Least_Squares_XY_Sample_Vector_Fill_In(N_CALIBRATION_TOTAL_SAMPLES);
    Least_Squares_X2_Sample_Vector_Fill_In(N_CALIBRATION_TOTAL_SAMPLES);
	
	Least_Squares_Sum_X_Calculate(N_CALIBRATION_TOTAL_SAMPLES);
	Least_Squares_Sum_Y_Calculate(N_CALIBRATION_TOTAL_SAMPLES);
	Least_Squares_Sum_XY_Calculate(N_CALIBRATION_TOTAL_SAMPLES);
	Least_Squares_Sum_X2_Calculate(N_CALIBRATION_TOTAL_SAMPLES);
	Least_Squares_Sum_2_X_Calculate();	
	
	division_by_zero = Least_Squares_Linear_Slope_Calculate(N_CALIBRATION_TOTAL_SAMPLES);
	if(division_by_zero == 1){
		cli();
		system_flags |= (1 << CALIBRATION_ERROR_FLAG);
		sei();
	}
	
	division_by_zero = Least_Squares_Linear_Offset_Calculate(N_CALIBRATION_TOTAL_SAMPLES);
	if(division_by_zero == 1){
		cli();
		system_flags |= (1 << CALIBRATION_ERROR_FLAG);
		sei();
	}
	
	calibration_slope = Least_Squares_Get_Linear_Slope();
	calibration_offset = Least_Squares_Get_Linear_Offset();
}