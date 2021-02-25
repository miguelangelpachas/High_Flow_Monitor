
/* File inclusion */
#include "stdint.h"
#include "project_defines.h"
#include "filters.h"

/* Global variables */

static float flow_lpf_output = 0;
static float fio2_lpf_output = 0;
static float battery_lpf_output = 0;

/* Function definition */

void Filters_Initialize(void){

	Proximal_Flow_RC_LPF(0.0, 1);
	FiO2_RC_LPF(0.0, 1);
	Battery_RC_LPF(0.0, 1);

}

float Proximal_Flow_RC_LPF(float raw_value, uint8_t restart){

	static float lpf_prev_out = 0.0;
	float lpf_out, sample_period_sec, div;
	
	if(restart){
		lpf_prev_out = 0.0;
		lpf_out = 0.0;
		}else{
		sample_period_sec = 0.001 * ((float)FLOW_SENSOR_SAMPLE_PERIOD_MS);
		div = sample_period_sec + FLOW_LPF_RC_CONSTANT_S;
		
		lpf_out = (raw_value * (sample_period_sec / div)) +  (lpf_prev_out * (FLOW_LPF_RC_CONSTANT_S / div));
		lpf_prev_out = lpf_out;
	}
	
	return lpf_out;
}


float FiO2_RC_LPF(float raw_value, uint8_t restart){

	static float lpf_prev_out = 0.0;
	float lpf_out, sample_period_sec, div;
	
	if(restart){
		lpf_prev_out = 0.0;
		lpf_out = 0.0;
		}else{
		sample_period_sec = 0.001 * ((float)FIO2_SENSOR_SAMPLE_PERIOD_MS);
		div = sample_period_sec + FIO2_LPF_RC_CONSTANT_S;
		
		lpf_out = (raw_value * (sample_period_sec / div)) +  (lpf_prev_out * (FIO2_LPF_RC_CONSTANT_S / div));
		lpf_prev_out = lpf_out;
	}
	
	return lpf_out;
}


float Battery_RC_LPF(float raw_value, uint8_t restart){

	static float lpf_prev_out = 0.0;
	float lpf_out, sample_period_sec, div;
	
	if(restart){
		lpf_prev_out = 0.0;
		lpf_out = 0.0;
	}else{
		sample_period_sec = 0.001 * ((float)BATT_SENSOR_SAMPLE_PERIOD_MS);
		div = sample_period_sec + BATT_LPF_RC_CONSTANT_S;
		
		lpf_out = (raw_value * (sample_period_sec / div)) +  (lpf_prev_out * (BATT_LPF_RC_CONSTANT_S / div));
		lpf_prev_out = lpf_out;
	}
	
	return lpf_out;
}




void Apply_Flow_LPF(float flow_signal){
	flow_lpf_output = Proximal_Flow_RC_LPF(flow_signal, 0);
}


void Apply_FiO2_LPF(float fio2_signal){
	fio2_lpf_output = FiO2_RC_LPF(fio2_signal, 0);
}


void Apply_Battery_LPF(float batt_signal){
	battery_lpf_output = Battery_RC_LPF(batt_signal, 0);
}


float Get_Flow_LPF_Output(void){
	return  flow_lpf_output;
}


float Get_FiO2_LPF_Output(void){
	return  fio2_lpf_output;
}

float Get_Battery_LPF_Output(void){
	return  battery_lpf_output;
}
