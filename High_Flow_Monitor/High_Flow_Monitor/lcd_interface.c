
/* File inclusion */
#include <avr/interrupt.h>
#include <stdint.h>
#include "MT_cog_lcd.h"
#include "lcd_interface.h"
#include "global.h"


/* Variable definition */

// Strings for the boot screen
const char lcd_system_boot_msg_01[] = "   BIENVENIDO   ";
const char lcd_system_boot_msg_02[] = "                ";

// Strings for calibration start
const char lcd_system_calibration_mode_msg_01[] = "   INICIANDO    ";
const char lcd_system_calibration_mode_msg_02[] = "  CALIBRACION   ";

// Strings for calibration processing	
const char lcd_system_calibration_process_msg_01[] = "   PROCESANDO   ";
const char lcd_system_calibration_process_msg_02[] = "   DATOS...     ";

// Strings for calibration sensor error
const char lcd_system_calibration_sensor_error_msg_01[] = "  ERROR SENSOR  ";
const char lcd_system_calibration_sensor_error_msg_02[] = "     DE FiO2    ";

// Strings for calibration error
const char lcd_system_calibration_error_msg_01[] = "  ERROR EN LA   ";
const char lcd_system_calibration_error_msg_02[] = "  CALIBRACION   ";

// Strings for calibration complete
const char lcd_calibration_complete_msg_01[] = "  CALIBRACION   ";
const char lcd_calibration_complete_msg_02[] = "   COMPLETA     ";

// Strings for calibration
//const char lcd_calibration_header_01[] = "Entre   % y   % ";
//const char lcd_calibration_header_02[] = "   FiO2:   %    ";
const char lcd_calibration_header_01[] = "Entre ";
const char lcd_calibration_header_02[] = "% y ";
const char lcd_calibration_header_03[] = "% ";
const char lcd_calibration_header_04[] = "   FiO2: ";
const char lcd_calibration_header_05[] = "%    ";
char lcd_calibration_upper_boundary[] = "00";
char lcd_calibration_lower_boundary[] = "00";
char lcd_calibration_reference_value[] = "00";

uint8_t lcd_calibration_upper_boundary_array[N_CALIBRATION_REFERENCE_POINTS] = {
																				CALIB_UPPER_BOUNDARY_01,
																				CALIB_UPPER_BOUNDARY_02,
																				CALIB_UPPER_BOUNDARY_03,
																				CALIB_UPPER_BOUNDARY_04,
																				CALIB_UPPER_BOUNDARY_05,
																				CALIB_UPPER_BOUNDARY_06,
																				CALIB_UPPER_BOUNDARY_07,
																				CALIB_UPPER_BOUNDARY_08
																				};
																				
uint8_t lcd_calibration_lower_boundary_array[N_CALIBRATION_REFERENCE_POINTS] = {
																				CALIB_LOWER_BOUNDARY_01,
																				CALIB_LOWER_BOUNDARY_02,
																				CALIB_LOWER_BOUNDARY_03,
																				CALIB_LOWER_BOUNDARY_04,
																				CALIB_LOWER_BOUNDARY_05,
																				CALIB_LOWER_BOUNDARY_06,
																				CALIB_LOWER_BOUNDARY_07,
																				CALIB_LOWER_BOUNDARY_08
																				};

// Strings for shutdown message 
const char lcd_system_shutdown_msg_01[] = "SUELTE EL BOTON ";
const char lcd_system_shutdown_msg_02[] = "  PARA APAGAR   ";

// Strings for configuration mode
char lcd_config_flow_header_active[] = " LPM MIN:    ";
char lcd_config_flow_header_unactive[] = " LPM MIN:    ";
char lcd_config_fio2_header_active[] = " FiO2 MIN:    ";
char lcd_config_fio2_header_unactive[] = " FiO2 MIN:   ";
char lcd_config_flow_min_value[] = "000 ";
char lcd_config_fio2_min_value[] = "000 ";

// Strings for alarm messages
const char lcd_low_flow_msg[] = "FLUJO MUY BAJO! ";
const char lcd_low_fio2_msg[] = "FiO2 MUY BAJO!  ";

// Strings for normal operation mode
const char lcd_flow_header[] ="LPM : ";
const char lcd_fio2_header[] = "FiO2: ";
const char lcd_flow_units[] = "    ";
const char lcd_fio2_units[] = "%   ";
const char lcd_flow_error[]="LPM :ERROR   ";
const char lcd_fio2_error[]="FiO2:ERROR   ";
char lcd_flow_str[]="000 ";
char lcd_fio2_str[]="000 ";

// Strings for alarm messages
const char lcd_battery_header[] = "BAT";
const char lcd_battery_units[] = "%";
char lcd_battery_str[]="00";

// Battery charging gauge
const char batt_charge_gauge[BATT_CHARGE_GAUGE_STATES][4]={
										{' ', ' ', ' ', '\0'},
										{'.', ' ', ' ', '\0'},
										{'.', '.', ' ', '\0'},
										{'.', '.', '.', '\0'}
									};

// Battery charging gauge index
static uint8_t batt_charge_gauge_index = 0;


/* Variable definition */
uint16_t battery_level_display_value = 0;
uint16_t flow_threshold_display_value = 0, fio2_threshold_display_value = 0;
uint16_t flow_display_value = 0, fio2_display_value = 0;
static uint16_t prev_flow_display_value = 0, prev_fio2_display_value = 0;

/* Function declaration */
void U16_To_Str(uint16_t val, char *str, uint8_t n_digits);
void Remove_Non_Significant_Digits(uint16_t val, char *str);


void LCD_Interface_Initialize(void){
	
	lcd_config_flow_header_active[0]=0x7E;
	lcd_config_fio2_header_active[0]=0x7E;
	
	COG_LCD_Init();
	LCD_Print_Boot_Screen();
		
	/* Configure LCD_BACKLIGHT_ENABLE pin as output  */
	DDR_LCD_BACKLIGHT_ENABLE |= (1 << LCD_BACKLIGHT_ENABLE);
	PORT_LCD_BACKLIGHT_ENABLE |= (1 << LCD_BACKLIGHT_ENABLE);
}


void LCD_Update_Flow_Display(float flow_f){
	
	if(flow_f < 0){
		flow_f = -flow_f;
	}
	
	if(flow_f > PROXIMAL_FLOW_SENSOR_MAX_VALUE){
		flow_f = PROXIMAL_FLOW_SENSOR_MAX_VALUE;
	}
	
	prev_flow_display_value = flow_display_value;
	flow_display_value = (uint16_t)flow_f;
	
	//if(flow_display_value != prev_flow_display_value){
		LCD_Print_Flow_Header();
		LCD_Print_Flow_Value();
		LCD_Print_Flow_Units();
	//}
	
}



void LCD_Update_FiO2_Display(float fio2_f){
		
	if(fio2_f > 100.0){
		fio2_f = 100.0;
	}else if(fio2_f < 0.0){
		fio2_f = 0.0;
	}else{
		//Does nothing 
	}
	
	prev_fio2_display_value = fio2_display_value;
	fio2_display_value = (uint16_t)fio2_f;
	
	//if(fio2_display_value != prev_fio2_display_value){
		LCD_Print_FiO2_Header();
		LCD_Print_FiO2_Value();
		LCD_Print_FiO2_Units();
	//}
	
}



void LCD_Print_Flow_Header(void){
	COG_LCD_Goto_XY(1,1);
	COG_LCD_Put_String(lcd_flow_header);
}


void LCD_Print_Flow_Units(void){
	COG_LCD_Goto_XY(10,1);
	COG_LCD_Put_String(lcd_flow_units);	
}


void LCD_Print_Flow_Value(void){
	U16_To_Str(flow_display_value, lcd_flow_str, 3);
	Remove_Non_Significant_Digits(flow_display_value, lcd_flow_str);
	COG_LCD_Goto_XY(6,1);
	COG_LCD_Put_String(lcd_flow_str);
}


void LCD_Print_Flow_Error(void){
	COG_LCD_Goto_XY(1,1);
	COG_LCD_Put_String(lcd_flow_error);
}


void LCD_Print_FiO2_Header(void){
	COG_LCD_Goto_XY(1,2);
	COG_LCD_Put_String(lcd_fio2_header);
}


void LCD_Print_FiO2_Units(void){
	COG_LCD_Goto_XY(10,2);
	COG_LCD_Put_String(lcd_fio2_units);	
}


void LCD_Print_FiO2_Value(void){
	U16_To_Str(fio2_display_value, lcd_fio2_str, 3);
	Remove_Non_Significant_Digits(fio2_display_value, lcd_fio2_str);
	COG_LCD_Goto_XY(6,2);
	COG_LCD_Put_String(lcd_fio2_str);
	
}


void LCD_Print_FiO2_Error(void){
	COG_LCD_Goto_XY(1,2);
	COG_LCD_Put_String(lcd_fio2_error);
}



void U16_To_Str(uint16_t val, char *str, uint8_t n_digits){
	
	uint16_t i;
	
	i = n_digits;
	
	while(i > 0){
		i--;
		*(str + i) = (val % 10) + 48;
		val /= 10;
	}
}

	
void Remove_Non_Significant_Digits(uint16_t val, char *str){
	
	if(val < 100){
		*str = ' ';
	}
	
	if(val < 10){
		*(str+ 1) = ' ';
	}	
	
}



void LCD_Print_Flow_Boundaries_Error_Message(void){
	COG_LCD_Goto_XY(1,1);
	COG_LCD_Put_String(lcd_low_flow_msg);	
}


void LCD_Print_FiO2_Boundaries_Error_Message(void){
	COG_LCD_Goto_XY(1,2);
	COG_LCD_Put_String(lcd_low_fio2_msg);	
}


void LCD_Print_Boundaries_Error_Screen(void){
	
	if(system_flags & (1 << FLOW_ALARM_FLAG	)){
		LCD_Print_Flow_Boundaries_Error_Message();
	}else{
		COG_LCD_Goto_XY(1,1);
		COG_LCD_Put_String("                ");		
	}
	
	if(system_flags & (1 << FIO2_ALARM_FLAG)){
		LCD_Print_FiO2_Boundaries_Error_Message();
	}else{
		COG_LCD_Goto_XY(1,2);
		COG_LCD_Put_String("                ");
	}
}


void LCD_Update_Flow_Threshold(float flow_f){
	
	if(flow_f > PROXIMAL_FLOW_SENSOR_MAX_VALUE){
		flow_threshold_display_value = (uint16_t)PROXIMAL_FLOW_SENSOR_MAX_VALUE;
	}else if(flow_f < 0){
		flow_threshold_display_value = 0;
	}else{
		flow_threshold_display_value = (uint16_t)flow_f;
	}
	
	U16_To_Str(flow_threshold_display_value, lcd_config_flow_min_value, 3);
	Remove_Non_Significant_Digits(flow_threshold_display_value, lcd_config_flow_min_value);
	
}


void LCD_Update_FiO2_Threshold(float fio2_f){

	if(fio2_f > 100){
		fio2_threshold_display_value = 100;
	}else if(fio2_f < 0){
		fio2_threshold_display_value = 0;
	}else{
		fio2_threshold_display_value = (uint16_t)fio2_f;
	}
	
	U16_To_Str(fio2_threshold_display_value, lcd_config_fio2_min_value, 3);
	Remove_Non_Significant_Digits(fio2_threshold_display_value, lcd_config_fio2_min_value);
	
}


void LCD_Print_Flow_Threshold_Config_Header(void){
	
	if(config_state == FLOW_THRESHOLD_ADJUST_STATE){
		COG_LCD_Goto_XY(1,1);
		COG_LCD_Put_String(lcd_config_flow_header_active);	
	}else if(config_state == FIO2_THRESHOLD_ADJUST_STATE){
		COG_LCD_Goto_XY(1,1);
		COG_LCD_Put_String(lcd_config_flow_header_unactive);		
	}else{
		// Does nothing
	}
	
}


void LCD_Print_FiO2_Threshold_Config_Header(void){
	
	if(config_state == FLOW_THRESHOLD_ADJUST_STATE){
		COG_LCD_Goto_XY(1,2);
		COG_LCD_Put_String(lcd_config_fio2_header_unactive);
	}else if(config_state == FIO2_THRESHOLD_ADJUST_STATE){
		COG_LCD_Goto_XY(1,2);
		COG_LCD_Put_String(lcd_config_fio2_header_active);
	}else{
		// Does nothing
	}	
}


void LCD_Print_Flow_Threshold_Config_Value(void){
	COG_LCD_Goto_XY(13,1);
	COG_LCD_Put_String(lcd_config_flow_min_value);
}


void LCD_Print_FiO2_Threshold_Config_Value(void){
	COG_LCD_Goto_XY(13,2);
	COG_LCD_Put_String(lcd_config_fio2_min_value);	
}

void LCD_Print_Config_Screen(void){
	LCD_Print_Flow_Threshold_Config_Header();
	LCD_Print_FiO2_Threshold_Config_Header();
	LCD_Print_Flow_Threshold_Config_Value();
	LCD_Print_FiO2_Threshold_Config_Value();
}


void LCD_Print_Shutdown_Message(void){
	COG_LCD_Goto_XY(1,1);
	COG_LCD_Put_String(lcd_system_shutdown_msg_01);	
	COG_LCD_Goto_XY(1,2);
	COG_LCD_Put_String(lcd_system_shutdown_msg_02);	
}


void LCD_Backlight_On(void){
	PORT_LCD_BACKLIGHT_ENABLE |= (1 << LCD_BACKLIGHT_ENABLE);
}


void LCD_Backlight_Off(void){
	PORT_LCD_BACKLIGHT_ENABLE &= ~(1 << LCD_BACKLIGHT_ENABLE);
}


void LCD_Print_Battery_Header(void){
	COG_LCD_Goto_XY(14,1);
	COG_LCD_Put_String(lcd_battery_header);	
}


void LCD_Print_Battery_Units(void){	
	COG_LCD_Goto_XY(16,2);
	COG_LCD_Put_String(lcd_battery_units);
}

void LCD_Print_Battery_Value(void){
	U16_To_Str(battery_level_display_value, lcd_battery_str, 2);
	//Remove_Non_Significant_Digits(battery_level_display_value, lcd_battery_str);
	COG_LCD_Goto_XY(14,2);
	COG_LCD_Put_String(lcd_battery_str);
}


void LCD_Update_Battery_Level_Display(float batt_percent){
	
	if(batt_percent < 0){
		batt_percent = 0;
	}else if(batt_percent > 99){
		batt_percent = 99;
	}else{
		//Does nothing 
	}
	
	battery_level_display_value = (uint16_t)batt_percent;
	
	LCD_Print_Battery_Header();
	LCD_Print_Battery_Units();
	LCD_Print_Battery_Value();
	
}

void LCD_Print_Battery_Charging_Gauge(void){
	COG_LCD_Goto_XY(14,2);
	COG_LCD_Put_String(batt_charge_gauge[batt_charge_gauge_index]);
}


void LCD_Update_Battery_Charging_Gauge(void){
	
	LCD_Print_Battery_Header();
	LCD_Print_Battery_Charging_Gauge();
	
	if(system_flags & (1 << BATTERY_CHARGE_GAUGE_FLAG)){
		
		cli();
		system_flags &= ~(1 << BATTERY_CHARGE_GAUGE_FLAG);
		sei();
		
		batt_charge_gauge_index++;	
		if(batt_charge_gauge_index >= BATT_CHARGE_GAUGE_STATES){
			batt_charge_gauge_index = 0;
		}		
	}
	
}


void Battery_Charge_Gauge_Index_Reset(void){
	batt_charge_gauge_index = 0;
}



void LCD_Print_Boot_Screen(void){
	COG_LCD_Goto_XY(1,1);
	COG_LCD_Put_String(lcd_system_boot_msg_01);	
	COG_LCD_Goto_XY(1,2);
	COG_LCD_Put_String(lcd_system_boot_msg_02);
}


void LCD_Print_Calibration_Start_Screen(void){
	COG_LCD_Goto_XY(1,1);
	COG_LCD_Put_String(lcd_system_calibration_mode_msg_01);
	COG_LCD_Goto_XY(1,2);
	COG_LCD_Put_String(lcd_system_calibration_mode_msg_02);
}

void LCD_Print_Calibration_Processing_Screen(void){
	COG_LCD_Goto_XY(1,1);
	COG_LCD_Put_String(lcd_system_calibration_process_msg_01);
	COG_LCD_Goto_XY(1,2);
	COG_LCD_Put_String(lcd_system_calibration_process_msg_02);	
}


void LCD_Print_Calibration_Sensor_Error_Screen(void){
	COG_LCD_Goto_XY(1,1);
	COG_LCD_Put_String(lcd_system_calibration_sensor_error_msg_01);
	COG_LCD_Goto_XY(1,2);
	COG_LCD_Put_String(lcd_system_calibration_sensor_error_msg_02);
}


void LCD_Print_Calibration_Error_Screen(void){
	COG_LCD_Goto_XY(1,1);
	COG_LCD_Put_String(lcd_system_calibration_error_msg_01);
	COG_LCD_Goto_XY(1,2);
	COG_LCD_Put_String(lcd_system_calibration_error_msg_02);	
}


void LCD_Print_Calibration_Complete_Screen(void){
	COG_LCD_Goto_XY(1,1);
	COG_LCD_Put_String(lcd_calibration_complete_msg_01);
	COG_LCD_Goto_XY(1,2);
	COG_LCD_Put_String(lcd_calibration_complete_msg_02);	
}


void LCD_Print_Calibration_Range_Header(uint8_t index){
	
	COG_LCD_Goto_XY(1,1);
	COG_LCD_Put_String(lcd_calibration_header_01);
		
	U16_To_Str(lcd_calibration_lower_boundary_array[index], lcd_calibration_lower_boundary, 2);
	COG_LCD_Goto_XY(7,1);
	COG_LCD_Put_String(lcd_calibration_lower_boundary);
	
	COG_LCD_Goto_XY(9,1);
	COG_LCD_Put_String(lcd_calibration_header_02);
	
	U16_To_Str(lcd_calibration_upper_boundary_array[index], lcd_calibration_upper_boundary, 2);
	COG_LCD_Goto_XY(13,1);
	COG_LCD_Put_String(lcd_calibration_upper_boundary);	

	COG_LCD_Goto_XY(15,1);
	COG_LCD_Put_String(lcd_calibration_header_03);	
	
}


void LCD_Print_Calibration_Reference_Header(void){
	COG_LCD_Goto_XY(1,2);
	COG_LCD_Put_String(lcd_calibration_header_04);
}


void LCD_Print_Calibration_Reference_Value(uint8_t ref_fio2){
	U16_To_Str((uint16_t)ref_fio2, lcd_calibration_reference_value, 2);
	COG_LCD_Goto_XY(10,2);
	COG_LCD_Put_String(lcd_calibration_reference_value);
}


void LCD_Print_Calibration_Reference_Units(void){
	COG_LCD_Goto_XY(12,2);
	COG_LCD_Put_String(lcd_calibration_header_05);
}


