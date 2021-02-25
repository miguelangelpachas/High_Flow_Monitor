
#ifndef LCD_INTERFACE_H_
#define LCD_INTERFACE_H_

void LCD_Interface_Initialize(void);
void LCD_Update_Flow_Display(float flow_f);
void LCD_Update_FiO2_Display(float fio2_f);
void LCD_Update_Battery_Level_Display(float batt_percent);
void LCD_Print_Flow_Header(void);
void LCD_Print_Flow_Units(void);
void LCD_Print_Flow_Value(void);
void LCD_Print_FiO2_Header(void);
void LCD_Print_FiO2_Units(void);
void LCD_Print_FiO2_Value(void);
void LCD_Print_Flow_Error(void);
void LCD_Print_FiO2_Error(void);
void LCD_Print_Battery_Header(void);
void LCD_Print_Battery_Units(void);
void LCD_Print_Battery_Value(void);
void LCD_Print_Flow_Header(void);
void LCD_Print_Boundaries_Error_Screen(void);
void LCD_Print_Flow_Boundaries_Error_Message(void);
void LCD_Print_FiO2_Boundaries_Error_Message(void);

void LCD_Update_Flow_Threshold(float flow_f);
void LCD_Update_FiO2_Threshold(float fio2_f);
void LCD_Print_Flow_Threshold_Config_Header(void);
void LCD_Print_FiO2_Threshold_Config_Header(void);
void LCD_Print_Flow_Threshold_Config_Value(void);
void LCD_Print_FiO2_Threshold_Config_Value(void);
void LCD_Print_Config_Screen(void);
void LCD_Print_Shutdown_Message(void);

void LCD_Update_Battery_Charging_Gauge(void);
void Battery_Charge_Gauge_Index_Reset(void);
void LCD_Print_Battery_Charging_Gauge(void);

void LCD_Print_Boot_Screen(void);
void LCD_Print_Calibration_Start_Screen(void);
void LCD_Print_Calibration_Complete_Screen(void);
void LCD_Print_Calibration_Range_Header(uint8_t index);
void LCD_Print_Calibration_Reference_Header(void);
void LCD_Print_Calibration_Reference_Value(uint8_t ref_fio2);
void LCD_Print_Calibration_Reference_Units(void);
void LCD_Print_Calibration_Processing_Screen(void);
void LCD_Print_Calibration_Sensor_Error_Screen(void);
void LCD_Print_Calibration_Error_Screen(void);

void LCD_Backlight_On(void);
void LCD_Backlight_Off(void);

#endif