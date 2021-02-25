
#ifndef HIGH_FLOW_MONITOR_H_
#define HIGH_FLOW_MONITOR_H_

/* Type definition */
typedef enum{
	Waiting_For_Button_Press_Long,
	Waiting_For_Button_Release,
	System_Shut_Down
}Button_State_t;

/* Function declaration (prototypes) */

void System_Initialize(void);
void SPI_Initialize(void);
void TWI_Initialize(void);
void Timer0_Initialize(void);
void Timer1_Initialize(void);
void Sensors_Read_And_Filter(void);
void Numeric_Displays_Update(void);
void ADC_Initialize(void);

void Normal_Operation_Mode(void);
void Alarm_Config_Mode(void);
void Alarm_Flow_Threshold_Increase(void);
void Alarm_Flow_Threshold_Set(void);
void Alarm_FiO2_Threshold_Increase(void);
void Alarm_FiO2_Threshold_Set(void);
void System_Shutdown_Message(void);

void Save_Flow_Alarm_Threshold_Index_In_EEPROM(uint8_t new_threshold);
void Save_FiO2_Alarm_Threshold_Index_In_EEPROM(uint8_t new_threshold);
void Load_Flow_Alarm_Threshold_Index_From_EEPROM(void);
void Load_FiO2_Alarm_Threshold_Index_From_EEPROM(void);

void WDT_Off(void);
void WDT_Initialize(void);


#endif

