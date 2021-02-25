/*
 * High_Flow_Monitor.c
 *
 * Created: 30/08/2020 15:20:01
 * Author : mtorres
 */ 

/* File inclusion */
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <stdint.h>
#include "project_defines.h"
#include <util/delay.h>
#include "global.h"
#include "lcd_interface.h"
#include "high_flow_monitor.h"
#include "filters.h"
#include "MT_sensirion_sfm3000.h"
#include "mt_debounce.h"
#include "MT_cog_lcd.h"	

/* Main function */
int main(void)
{
	uint8_t counter_100ms = 0;
	uint8_t dummy = 0;
	
	/* Disable watchdog timer */
	WDT_Off();
	/* Initialize system */
	System_Initialize();
	/* Enable interrupts */
	sei();
	/* Wait for a moment */
	_delay_ms(250);
	
	/* Calibration parameters by default */
	#if (FIRST_TIME_PROGRAMMING == 1)
	Save_FiO2_Calib_Line_Slope_In_EEPROM(FIO2_CALIB_LINE_SLOPE_BY_DEFAULT);
	Save_FiO2_Calib_Line_Offset_In_EEPROM(FIO2_CALIBE_LINE_OFFSET_BY_DEFAULT);
	Save_Flow_Alarm_Threshold_Index_In_EEPROM(FLOW_THRESHOLD_INDEX_BY_DEFAULT);
	Save_FiO2_Alarm_Threshold_Index_In_EEPROM(FIO2_THRESHOLD_INDEX_BY_DEFAULT);
	#endif
	
	/* Procedure to enter to the calibration mode */
	while(TWO_BUTTONS_PRESSED && (counter_100ms < 20)){
		counter_100ms++;
		_delay_ms(100);
	}
	/* If both buttons stay pressed for 2 seconds, then the system enters to CALIBRATION MODE */
	if(counter_100ms == 20){
		system_flags |= (1 << CALIBRATION_MODE_FLAG);
	}

	/* Check if the user wants to perform a calibration of the oxygen sensor */
	if(system_flags & (1 << CALIBRATION_MODE_FLAG)){
		cli();
		system_flags &= ~(1 << CALIBRATION_MODE_FLAG);
		sei();
		FiO2_Sensor_Calibration_Sequence();
	}else{
		/* If the buttons have been pressed during the previous screen, ignore them */
		dummy = G1_Get_Button_Press(1 << ALARM_ON_OFF_BUTTON);
		dummy = G1_Get_Button_Press(1 << SYSTEM_MENU_BUTTON);
		dummy = G1_Get_Button_Long(1 << ALARM_ON_OFF_BUTTON);
		dummy = G1_Get_Button_Long(1 << SYSTEM_MENU_BUTTON);
		dummy = G1_Get_Button_Release(1 << SYSTEM_ON_OFF_BUTTON);	
	}
		
	/* Load calibration parameters from EEPROM */
	Load_FiO2_Calib_Line_Slope_From_EEPROM();
	Load_FiO2_Calib_Line_Offset_From_EEPROM();
	
	/* Load alarm parameters from EEPROM */
	Load_Flow_Alarm_Threshold_Index_From_EEPROM();
	Load_FiO2_Alarm_Threshold_Index_From_EEPROM();
	
	/* Initialize Watchdog timer */
	WDT_Initialize();
	
	/* Repeat forever*/
    while (1) 
    {

		/* Read sensors if necessary */
		if(system_flags & (1 << READ_SENSORS_FLAG)){
			Sensors_Read_And_Filter();
			/* Clear the flag */
			cli();
			system_flags &= ~(1 << READ_SENSORS_FLAG);
			sei();
		}
		
		/* Menu visualization */
		
		if(system_flags & (1 << SYSTEM_SHUT_DOWN_FLAG)){
			
			System_Shutdown_Message();
			
		}else{
			
			if(system_flags & (1 << ALARM_CONFIG_MODE_FLAG)){
				Alarm_Config_Mode();	
			}else{
				Normal_Operation_Mode();
			}
		}
		
		/* Kick the dog!!! */
		wdt_reset();
	}
	
	return 0;
}



ISR(TIMER0_COMPA_vect){
	
	static uint16_t counter_2ms = 0;
	static uint8_t counter_10ms = 0;
	static uint8_t counter_250ms = 0;
	
	counter_2ms++;
	
	/* Read and filter sensors every 10 ms */
	if(counter_2ms >= 5){
		counter_2ms = 0;
		counter_10ms++;
		system_flags |= (1 << READ_SENSORS_FLAG);
		
		/* Scan push buttons every 10 ms */
		G1_Button_Scan();		
	}
	
	/* Refresh LCD every 250 ms */
	if(counter_10ms >= 25){
		counter_10ms = 0;
		counter_250ms++;
		system_flags |= (1 << DISPLAY_UPDATE_FLAG);
	}
	
	/* Battery charge gauge changes every 500 ms */
	if(counter_250ms >= 2){
		counter_250ms = 0;
		system_flags |= (1 << BATTERY_CHARGE_GAUGE_FLAG);
	}
	
}




