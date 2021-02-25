/* Inclusión de archivos */
#include <avr/io.h>
#include "project_defines.h"
#include <util/delay.h>
#include "MT_spi_master.h"
#include "MT_cog_lcd.h"


/* Definición de funciones */

void COG_LCD_Init(void){
	
	/* Configurar los pines de control como salida e inicializarlos */

	DDR_COG_LCD_RST |= (1 << COG_LCD_RST);
	PORT_COG_LCD_RST |= (1 << COG_LCD_RST);
	
	DDR_COG_LCD_RS |= (1 << COG_LCD_RS);
	PORT_COG_LCD_RS &= ~(1 << COG_LCD_RS);
	
	DDR_COG_LCD_CSB |= (1 << COG_LCD_CSB);
	PORT_COG_LCD_CSB |= (1 << COG_LCD_CSB);
	
	/* Resetear la pantalla */
	PORT_COG_LCD_RST &= ~(1 << COG_LCD_RST);
	_delay_ms(2);
	PORT_COG_LCD_RST |= (1 << COG_LCD_RST);
	
	/* Esperar un tiempo ... */
	_delay_ms(20);

	/* Secuencia de inicialización */
	COG_LCD_Tx_Command(0x30);	// Despertar la pantalla
	_delay_ms(2);
	COG_LCD_Tx_Command(0x30);	// Despertar la pantalla
	COG_LCD_Tx_Command(0x30);	// Despertar la pantalla
	COG_LCD_Tx_Command(0x39);	// Función de configuración
	COG_LCD_Tx_Command(0x14);	// Frecuencia del oscilador interno
	COG_LCD_Tx_Command(0x56);	// Control de energía
	COG_LCD_Tx_Command(0x6D);	// Control del seguidor
	COG_LCD_Tx_Command(0x70);	// Contraste
	COG_LCD_Tx_Command(LCD_DISP_ON);	// Encender display
	COG_LCD_Tx_Command(LCD_ENTRY_INC);	// Modo entrada
	COG_LCD_Tx_Command(LCD_CLEAR);		// Limpiar pantalla
	_delay_ms(10);

}



void COG_LCD_Tx_Command(uint8_t command){

	uint8_t aux;

	PORT_COG_LCD_CSB &= ~(1 << COG_LCD_CSB);
	PORT_COG_LCD_RS &= ~(1 << COG_LCD_RS);
		
	aux= SPI_Master_Tx_Byte(command);

	PORT_COG_LCD_RS |= (1 << COG_LCD_RS);
	PORT_COG_LCD_CSB |= (1 << COG_LCD_CSB);
	
}


void COG_LCD_Put_Char(char byte){
	
	uint8_t aux;

	PORT_COG_LCD_CSB &= ~(1 << COG_LCD_CSB);
	PORT_COG_LCD_RS |= (1 << COG_LCD_RS);

	aux= SPI_Master_Tx_Byte(byte);

	PORT_COG_LCD_CSB |= (1 << COG_LCD_CSB);

}



void COG_LCD_Put_String(char *str){

	uint8_t indice = 0;

	while((indice < LCD_LINE_LENGTH) && (*str != '\0')){
		COG_LCD_Put_Char(*str);
		str++;
	}

}

void COG_LCD_Goto_XY(uint8_t pos_x, uint8_t pos_y){

	uint8_t comando, offset, col;

	/* Ubicar la dirección de la línea correspondiente */
	switch(pos_y){
		case 1:
		offset = LCD_START_LINE1;
		break;
		case 2:
		offset = LCD_START_LINE2;
		break;
		case 3:
		offset = LCD_START_LINE3;
		break;
		case 4:
		offset = LCD_START_LINE4;
		break;
		default:
		offset = LCD_START_LINE1;
		break;
	}

	/* Ubicar la dirección de la columna correspondiente*/
	if((pos_x >= 1) && (pos_x <= LCD_LINE_LENGTH)){
		col = pos_x - 1;
		}else{
		col = 0;
	}

	comando = (offset + col) | 0b10000000;
	COG_LCD_Tx_Command(comando);

}
