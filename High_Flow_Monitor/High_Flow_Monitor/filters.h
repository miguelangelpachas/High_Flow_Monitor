
#ifndef FILTERS_H_
#define FILTERS_H_

#include "stdint.h"

/* Function declaration */
void Filters_Initialize(void);

float Proximal_Flow_RC_LPF(float raw_value, uint8_t restart);
float FiO2_RC_LPF(float raw_value, uint8_t restart);
float Battery_RC_LPF(float raw_value, uint8_t restart);

void Apply_Flow_LPF(float flow_signal);
void Apply_FiO2_LPF(float fio2_signal);
void Apply_Battery_LPF(float batt_signal);

float Get_Flow_LPF_Output(void);
float Get_FiO2_LPF_Output(void);
float Get_Battery_LPF_Output(void);

#endif