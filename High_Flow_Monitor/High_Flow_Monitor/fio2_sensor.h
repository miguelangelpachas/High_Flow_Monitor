
#ifndef FIO2_SENSOR_H_
#define FIO2_SENSOR_H_

/* Function declaration */
void FiO2_Sensor_Read(void);
float Get_FiO2_Value(void);
void Save_FiO2_Calib_Line_Slope_In_EEPROM(float new_slope);
void Save_FiO2_Calib_Line_Offset_In_EEPROM(float new_offset);
void Load_FiO2_Calib_Line_Slope_From_EEPROM(void);
void Load_FiO2_Calib_Line_Offset_From_EEPROM(void);

#endif
