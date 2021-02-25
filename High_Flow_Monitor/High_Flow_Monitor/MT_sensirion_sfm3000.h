
#ifndef MT_SENSIRION_SFM3000_H_
#define MT_SENSIRION_SFM3000_H_

#include "stdint.h"

/* Function declaration */
void Proximal_Flow_Sensor_Initialize(void);
void Proximal_Flow_Sensor_Read(void);
//void Proximal_Flow_Sensor_Calculate_Volume(float flow_signal_slpm);
float Proximal_Flow_Sensor_Get_Flow(void);
//float Proximal_Flow_Sensor_Get_Volume(void);
//void Proximal_Flow_Sensor_Reset_Volume(void);
uint8_t Proximal_Flow_Sensor_Send_Command(uint16_t command);
uint16_t Proximal_Flow_Sensor_Get_Scale_Factor(void);
uint16_t Proximal_Flow_Sensor_Get_Offset(void);
uint8_t Proximal_Flow_Sensor_CRC_Check(uint8_t data[], uint8_t n_bytes, uint8_t checksum);

#endif