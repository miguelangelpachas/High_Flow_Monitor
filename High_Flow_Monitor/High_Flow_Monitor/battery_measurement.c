
#include <avr/io.h>
#include <avr/interrupt.h>
#include "project_defines.h"
#include "global.h"
#include "MT_adc.h"
#include "battery_measurement.h"

/* Variable definition */
static float battery_volt = 0;
static float battery_percent = 0;
static float battery_level = 0;
static float previous_battery_level = 0;


/* Function definition */

void Battery_Level_Read(void){

	uint16_t adc_value;
	
	/* Select the corresponding ADC channel*/
	ADC_Seleccionar_Canal(ADC_CHANNEL_BATTERY);
	/* Start a new conversion */
	ADCSRA |= (1 << ADSC);
	while(!(ADCSRA & (1 << ADIF))){
		/* Does nothing until the conversion is completed */
	}
	ADCSRA |= (1 << ADIF);
	
	/* Calculate the voltaje provided by the battery */
	adc_value = ADC;
	battery_volt = (((float)adc_value) * ADC_REFERENCE_VOLTAGE) / 1024.0;
	battery_volt /= BATT_AMP_GAIN;
	
	/* Limit the minimum value of the measure */
	if(battery_volt < BATT_MIN_OUT_V){
		battery_volt = BATT_MIN_OUT_V;
	}
	
	/* Detect if the battery is being charged */
	if(battery_volt > BATT_CHARGE_THRESHOLD_V){
		cli();
		system_flags |= (1 << BATTERY_CHARGING_FLAG);
		sei();
	}else{
		cli();
		system_flags &= ~(1 << BATTERY_CHARGING_FLAG);
		sei();		
	}

	/* Calculate the percentage of the battery that has been charged */
	battery_percent = 100.0 * (battery_volt - BATT_MIN_OUT_V) / (BATT_MAX_OUT_V - BATT_MIN_OUT_V);
  
	
	if(battery_percent > 99.0){
		battery_percent = 99.0;
	}else if(battery_percent < 0.0){
		battery_percent = 0.0;
	}else{
		// Does nothing
	}	
	
	
}



float Get_Battery_Level_Volt(void){
	return battery_volt;
}


float Get_Battery_Level_Percent(void){
	return battery_percent;
}