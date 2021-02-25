

/* Inclusion de archivos */
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include "mt_debounce.h"

/* Variables globales */
volatile uint8_t g1_button_state;	// debounced and inverted key state:
volatile uint8_t g1_button_press;	// key press detect
volatile uint8_t g1_button_rpt;	// key long press and repeat
volatile uint8_t g1_button_release; 



void Button_Debounce_Initialize(void) {
	DDR_BUTTON_G1 &= ~G1_ALL_BUTTON_MASK;
	PORT_BUTTON_G1 |= ((1 << BUTTON_02) | (1 << BUTTON_03));
}


uint8_t G1_Get_Button_Press(uint8_t button_mask) {
	cli();
	// read and clear atomic !
	button_mask &= g1_button_press; // read key(s)
	g1_button_press ^= button_mask; // clear key(s)
	sei();
	return button_mask;
}



uint8_t G1_Get_Button_Rpt(uint8_t button_mask) {
	cli();
	// read and clear atomic !
	button_mask &= g1_button_rpt; // read key(s)
	g1_button_rpt ^= button_mask; // clear key(s)
	sei();
	return button_mask;
}


uint8_t G1_Get_Button_Short(uint8_t button_mask) {
	cli();
	// read key state and key press atomic !
	return G1_Get_Button_Press(~g1_button_state & button_mask);
}


uint8_t G1_Get_Button_Long(uint8_t button_mask) {
	return G1_Get_Button_Press(G1_Get_Button_Rpt(button_mask));
}



uint8_t G1_Get_Button_Release(uint8_t button_mask) {
	cli();
	// read and clear atomic !
	button_mask &= g1_button_release; // read key(s)
	g1_button_release ^= button_mask; // clear key(s)
	sei();
	return button_mask;
}