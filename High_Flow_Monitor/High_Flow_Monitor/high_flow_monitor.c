
/* File inclusion */
#include <avr/io.h>
#include <stdint.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include "project_defines.h"
#include <util/delay.h>
#include "global.h"
#include "MT_spi_master.h"
#include "twi_master.h"
#include "MT_sensirion_sfm3000.h"
#include "high_flow_monitor.h"
#include "lcd_interface.h"
#include "MT_timer.h"
#include "filters.h"
#include "MT_adc.h"
#include "fio2_sensor.h"
#include "mt_debounce.h"
#include "battery_measurement.h"

/* Data stored in the EEPROM */
uint8_t EEMEM ee_flow_th_index = FLOW_THRESHOLD_INDEX_BY_DEFAULT;
uint8_t EEMEM ee_fio2_th_index = FIO2_THRESHOLD_INDEX_BY_DEFAULT;

/* Variable definition */
volatile uint16_t system_flags = 0;
volatile uint8_t  config_state = FLOW_THRESHOLD_ADJUST_STATE;
volatile Button_State_t button_state = Waiting_For_Button_Press_Long;

// Variables for alarm threshold configuration	
float flow_min_threshold[FLOW_THRESHOLD_N_VALUES] = {10.0, 20.0, 30.0, 40.0, 50.0};
float fio2_min_threshold[FIO2_THRESHOLD_N_VALUES] = {10.0, 15.0, 20.0, 25.0, 30.0};
uint8_t flow_th_index = FLOW_THRESHOLD_INDEX_BY_DEFAULT	;
uint8_t fio2_th_index = FIO2_THRESHOLD_INDEX_BY_DEFAULT;	

/* Function definition */

void System_Initialize(void){
	/* Configure SYSTEM_ON_OFF pin as output and keep the system ON */
	DDR_SYSTEM_ON_OFF |= (1 << SYSTEM_ON_OFF);
	PORT_SYSTEM_ON_OFF |= (1 << SYSTEM_ON_OFF);
	/* Configure alarm buzzer pin as output */
	DDR_ALARM_BUZZER |= (1<< ALARM_BUZZER);
	/* Turn-off alarm buzzer */
	PORT_ALARM_BUZZER &= ~(1 << ALARM_BUZZER);
	/*Alarm enabled by default */
	system_flags |= (1 << ALARM_ENABLED_FLAG);

	Button_Debounce_Initialize();
	Filters_Initialize();
	ADC_Initialize();
	SPI_Initialize();
	LCD_Interface_Initialize();
	TWI_Initialize();
	Proximal_Flow_Sensor_Initialize();
	
	/* Wait for a moment before reading the buttons */
	_delay_ms(250);
	
	Timer0_Initialize();
	Timer1_Initialize();
	
	LCD_Backlight_On();
}


void SPI_Initialize(void){

	spi_config_t my_spi;
	
	/* Configurar el módulo SPI para trabajar en Modo 3 a 1 Mhz */
	my_spi.mode = SPI_Master;
	my_spi.sck_prescaler = SCK_Fosc_32;
	my_spi.cpol = CPOL_1;
	my_spi.cpha = CPHA_1;
	my_spi.data_order = MSB_First;
	my_spi.interrupt = SPI_Int_Disabled;

	SPI_Configurar(&my_spi);
}


void TWI_Initialize(void){
	tw_init(TW_FREQ_250K, 0);
}


void Timer0_Initialize(void){
	
	Timer_8b_t timer0;
	
	/* Establecer los par?metros de configuracion del Timer0 */
	timer0.clock = Timer_Clk_PS1024;
	timer0.mode = Timer_8b_CTC_Mode;
	timer0.OCA = OC_Disabled;
	timer0.OCB = OC_Disabled;
	timer0.interrupt_mask = Timer_CompA_IE;
	timer0.OCRA = 16;
	timer0.OCRB = 0;
	/* Configurar el Timer0 */
	Timer0_Configurar(&timer0);
	
}


void Timer1_Initialize(void){

	Timer_16b_t timer1;
	
	/* Establecer los par?metros de configuracion del Timer1 */
	timer1.clock = Timer_Clk_PS256;
	timer1.mode = Timer_16b_CTC_OCRA_Mode;
	timer1.OCA = OC_Disabled;
	timer1.OCB = OC_Disabled;
	timer1.interrupt_mask = Timer_Interrupts_Disabled;
	timer1.OCRA = 15624;
	timer1.OCRB = 0;

	/* Configurar el Timer0 */
	Timer1_Configurar(&timer1);	

}



void Sensors_Read_And_Filter(void){
	
	float measure;
	
	/* Read an filter signals */
	
	Proximal_Flow_Sensor_Read();
	measure = Proximal_Flow_Sensor_Get_Flow();
	Apply_Flow_LPF(measure);
	
	FiO2_Sensor_Read();
	measure = Get_FiO2_Value();
	Apply_FiO2_LPF(measure);
	
	Battery_Level_Read();
	//Battery_Level_Apply_Hysteresis();
	//measure = Get_Battery_Level_Percent();
	//Apply_Battery_LPF(measure);	
	
	/* Check boundaries */
	measure = Get_Flow_LPF_Output();
	if(measure < flow_min_threshold[flow_th_index]){
		system_flags |= (1 << FLOW_ALARM_FLAG); 
	}else{
		system_flags &= ~(1 << FLOW_ALARM_FLAG); 
	}
	
	measure = Get_FiO2_LPF_Output();
	if(measure < fio2_min_threshold[fio2_th_index]){
		system_flags |= (1 << FIO2_ALARM_FLAG);
	}else{
		system_flags &= ~(1 << FIO2_ALARM_FLAG);
	}	
		
}


void Numeric_Displays_Update(void){
	
	float measure;

	if(system_flags & (1 << FLOW_SENSOR_ERROR_FLAG)){
		LCD_Print_Flow_Error();		
	}else{
		measure = Get_Flow_LPF_Output();
		LCD_Update_Flow_Display(measure);	
	}
	
	if(system_flags & (1 << FIO2_SENSOR_ERROR_FLAG)){
		LCD_Print_FiO2_Error();
	}else{	
		measure = Get_FiO2_LPF_Output();
		LCD_Update_FiO2_Display(measure);
	}
	
	if(system_flags & (1 << BATTERY_CHARGING_FLAG)){
		LCD_Update_Battery_Charging_Gauge();
	}else{
		measure = Get_Battery_Level_Percent();
		//measure = Get_Battery_Level_Stable();
		LCD_Update_Battery_Level_Display(measure);
		/* Reset the battery charge gauge index */
		Battery_Charge_Gauge_Index_Reset();
	}
	
}


void ADC_Initialize(void){
	
	ADC_t my_adc;

	my_adc.prescaler = ADC_Prescaler_64;
	my_adc.vref = AREF;
	my_adc.channel = ADC_Channel_0;
	my_adc.auto_trigger = ADC_Autotrigger_Disabled;
	my_adc.trigger_source = ADC_Free_Running_Mode;
	my_adc.interrupt_mask = ADC_Interrupt_Disabled;

	ADC_Configurar(&my_adc);
}


void Normal_Operation_Mode(void){
	
	/* Update message showed on screen */
	if(system_flags & (1 << DISPLAY_UPDATE_FLAG)){
		if(system_flags & (1 << ALARM_ENABLED_FLAG)){
			if(system_flags & BOUNDARIES_ERROR_MASK){
				LCD_Print_Boundaries_Error_Screen();
			}else{
				Numeric_Displays_Update();
			}
		}else{
			Numeric_Displays_Update();
		}		
		
		/* Clear the flag */		
		cli();
		system_flags &= ~(1 << DISPLAY_UPDATE_FLAG);
		sei();
	}
	
	/* Update alarm buzzer output */
	if(system_flags & (1 << ALARM_ENABLED_FLAG)){
		if((system_flags & SENSOR_ERROR_MASK) || (system_flags & BOUNDARIES_ERROR_MASK)){
			TCCR1A |= (1 << COM1A0);
		}else{
			TCCR1A &= ~(1 << COM1A0);
			PORT_ALARM_BUZZER &= ~(1 << ALARM_BUZZER);
		}
	}else{
		TCCR1A &= ~(1 << COM1A0);
		PORT_ALARM_BUZZER &= ~(1 << ALARM_BUZZER);
	}

	/* Enable/disable alarm */
	if(G1_Get_Button_Short(1 << ALARM_ON_OFF_BUTTON)){
		cli();
		system_flags ^= (1 << ALARM_ENABLED_FLAG);
		sei();
	}
	
	/* Change to configuration mode */
	if(G1_Get_Button_Short(1 << SYSTEM_MENU_BUTTON)){
		cli();
		system_flags |= (1 << ALARM_CONFIG_MODE_FLAG);
		sei();
	}
	
	/* Ignore short pressing on SYSTEM_MENU_BUTTON */
	if(G1_Get_Button_Long(1 << SYSTEM_MENU_BUTTON)){
		//Does nothing
	}	
	
	/* Prepare to shut down the system */
	if(G1_Get_Button_Release(1 << SYSTEM_ON_OFF_BUTTON)){
		cli();
		system_flags |= (1 << SYSTEM_SHUT_DOWN_FLAG);
		sei();
	}	

}


void Alarm_Config_Mode(void){
	
	/* Update message showed on screen */
	if(system_flags & (1 << DISPLAY_UPDATE_FLAG)){
		/* Update threshold values in the LCD interface */
		LCD_Update_Flow_Threshold(flow_min_threshold[flow_th_index]);
		LCD_Update_FiO2_Threshold(fio2_min_threshold[fio2_th_index]);
		/* Print configuration menu */
		LCD_Print_Config_Screen();
		/* Clear the flag */
		cli();
		system_flags &= ~(1 << DISPLAY_UPDATE_FLAG);
		sei();
	}	
	
	/* Adjust the threshold values with the alarm button */
	
	if(config_state == FLOW_THRESHOLD_ADJUST_STATE){
		
		if(G1_Get_Button_Short(1 << SYSTEM_MENU_BUTTON)){
			Alarm_Flow_Threshold_Increase();
		}
		
		if(G1_Get_Button_Long(1 << SYSTEM_MENU_BUTTON)){
			config_state = FIO2_THRESHOLD_ADJUST_STATE;
			cli();
			system_flags |= (1 << ALARM_CONFIG_MODE_FLAG);
			sei();			
		}
			
	}else if(config_state == FIO2_THRESHOLD_ADJUST_STATE){
		if(G1_Get_Button_Short(1 << SYSTEM_MENU_BUTTON)){
			Alarm_FiO2_Threshold_Increase();
		}
		
		if(G1_Get_Button_Long(1 << SYSTEM_MENU_BUTTON)){
			
			Save_Flow_Alarm_Threshold_Index_In_EEPROM(flow_th_index);
			Save_FiO2_Alarm_Threshold_Index_In_EEPROM(fio2_th_index);
			
			config_state = FLOW_THRESHOLD_ADJUST_STATE;
			cli();
			system_flags &= ~(1 << ALARM_CONFIG_MODE_FLAG);
			sei();
		}
			
	}else{
		// Does nothing
	}
	
	
	if(G1_Get_Button_Release(1 << SYSTEM_ON_OFF_BUTTON)){
		cli();
		system_flags |= (1 << SYSTEM_SHUT_DOWN_FLAG);
		sei();
	}
	
	/* Alarm buzzer off */
	TCCR1A &= ~(1 << COM1A0);
	PORT_ALARM_BUZZER &= ~(1 << ALARM_BUZZER);
	
}


void System_Shutdown_Message(void){
	
	if(system_flags & (1 << DISPLAY_UPDATE_FLAG)){	

		LCD_Print_Shutdown_Message();
		/* Clear the flag */
		cli();
		system_flags &= ~(1 << DISPLAY_UPDATE_FLAG);
		sei();
	}
	
	if(G1_Get_Button_Press(1 << SYSTEM_ON_OFF_BUTTON)){	
		/* Disable watchdog timer */
		WDT_Off();
		_delay_ms(250);	
		PORT_SYSTEM_ON_OFF &= ~(1 << SYSTEM_ON_OFF);
	}

}


void Alarm_Flow_Threshold_Increase(void){
	
	if(flow_th_index < (FLOW_THRESHOLD_N_VALUES - 1)){
		flow_th_index++;
	}else{
		flow_th_index = 0;
	}
	
}


void Alarm_FiO2_Threshold_Increase(void){

	if(fio2_th_index < (FIO2_THRESHOLD_N_VALUES - 1)){
		fio2_th_index++;
	}else{
		fio2_th_index = 0;
	}	
	
}



void Save_Flow_Alarm_Threshold_Index_In_EEPROM(uint8_t new_threshold){
	eeprom_busy_wait();
	eeprom_update_byte(&ee_flow_th_index, flow_th_index);	
}


void Save_FiO2_Alarm_Threshold_Index_In_EEPROM(uint8_t new_threshold){
	eeprom_busy_wait();
	eeprom_update_byte(&ee_fio2_th_index, fio2_th_index);	
}


void Load_Flow_Alarm_Threshold_Index_From_EEPROM(void){
	eeprom_busy_wait();
	flow_th_index = eeprom_read_byte(&ee_flow_th_index);
}


void Load_FiO2_Alarm_Threshold_Index_From_EEPROM(void){
	eeprom_busy_wait();
	fio2_th_index = eeprom_read_byte(&ee_fio2_th_index);
}


void WDT_Off(void){
	
	/* Disable interrupts */
	cli();
	/* Watchdog reset */
	wdt_reset();
	/* Clear WDRF in MCUSR */
	MCUSR &= ~(1 << WDRF);
	/* Write logical one to WDCE and  WDE */
	/* Keep old prescaler setting to prevent unintentional time-out */
	WDTCSR |= (1 << WDCE) | (1 << WDE);
	/* Turn off WDT */
	WDTCSR = 0x00;
	/* Enable interrupts */
	//sei();
}


void WDT_Initialize(void){
	
	cli();
	WDTCSR |= (1<< WDCE) | (1 <<WDE);
	/* Reset system after 250 ms*/
	WDTCSR |= (1 << WDE) | (1 << WDP3);
	sei();
	
}