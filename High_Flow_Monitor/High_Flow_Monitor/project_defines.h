
#ifndef PROJECT_DEFINES_H_
#define PROJECT_DEFINES_H_

/*****************************************************************************/
/***************************** PORTS AND PINS ********************************/
/*****************************************************************************/

/* System CPU */

#define F_CPU		8000000UL


/* Pins for SPI */

#define DDR_SPI		DDRB
#define PORT_SPI	PORTB
#define SS			2
#define SCK			5
#define MOSI		3
#define MISO		4


/* Pins for COG LCD */

#define DDR_COG_LCD_RST		DDRC
#define PORT_COG_LCD_RST	PORTC
#define COG_LCD_RST			1
		
#define DDR_COG_LCD_RS		DDRC
#define PORT_COG_LCD_RS		PORTC
#define COG_LCD_RS			2

#define DDR_COG_LCD_CSB		DDRC
#define PORT_COG_LCD_CSB	PORTC		
#define COG_LCD_CSB			3

/* Pins for push buttons */

#define DDR_ALARM_ON_OFF_BUTTON			DDRD	
#define PIN_ALARM_ON_OFF_BUTTON			PIND
#define PORT_ALARM_ON_OFF_BUTTON		PORTD
#define ALARM_ON_OFF_BUTTON				5

#define DDR_SYSTEM_ON_OFF_BUTTON		DDRD
#define PIN_SYSTEM_ON_OFF_BUTTON		PIND
#define PORT_SYSTEM_ON_OFF_BUTTON		PORTD
#define SYSTEM_ON_OFF_BUTTON			3

#define DDR_SYSTEM_MENU_BUTTON			DDRD
#define PIN_SYSTEM_MENU_BUTTON		    PIND
#define PORT_SYSTEM_MENU_BUTTON			PORTD
#define SYSTEM_MENU_BUTTON				4
		
/* Pins for alarm */

#define DDR_ALARM_BUZZER				DDRB
#define PORT_ALARM_BUZZER				PORTB
#define ALARM_BUZZER					1

/* Pin for shut-down signal */

#define	DDR_SYSTEM_ON_OFF				DDRD
#define PORT_SYSTEM_ON_OFF				PORTD
#define SYSTEM_ON_OFF					2

/* Pin for enable/disable backlight */

#define DDR_LCD_BACKLIGHT_ENABLE			DDRB
#define PORT_LCD_BACKLIGHT_ENABLE			PORTB
#define LCD_BACKLIGHT_ENABLE				0

/*****************************************************************************/
/************************** CONSTANTS AND MACROS *****************************/
/*****************************************************************************/

// ---------------------------- Defines for SPI --------------------------------

#define SPI_TX_BUFF_SIZE								2
#define SPI_RX_BUFF_SIZE								2

// ------------------------ Defines for ADC channels ---------------------------

#define ADC_CHANNEL_OXYGEN_SENSOR_VALUE					0
#define ADC_CHANNEL_OXYGEN_SENSOR_OFFSET				7
#define ADC_CHANNEL_BATTERY								6
#define ADC_REFERENCE_VOLTAGE							1.25

// ------------------- Defines for proximal flow sensor ------------------------

#define PROXIMAL_FLOW_SENSOR_SLAVE_ADDRESS              0x40
#define PROXIMAL_FLOW_SENSOR_SWA                        (PROXIMAL_FLOW_SENSOR_SLAVE_ADDRESS << 1)
#define PROXIMAL_FLOW_SENSOR_SRA                        (PROXIMAL_FLOW_SENSOR_SWA  | 1)

#define PROXIMAL_FLOW_SENSOR_OFFSET_SLPM                32000.0
#define PROXIMAL_FLOW_SENSOR_SCALE_FACTOR               142.8f
#define PROXIMAL_FLOW_SENSOR_THRESHOLD_SLPM             0.5f
#define PROXIMAL_FLOW_SENSOR_MAX_VALUE					200.0

#define PROXIMAL_FLOW_SENSOR_READ_PERIOD_MS             10

#define PROXIMAL_FLOW_SENSOR_CRC_POLYNOM                0x131
#define PROXIMAL_FLOW_SENSOR_COUNTER_MAX                10

#define SENSIRION_START_FLOW_MEASUREMENT                0x1000
#define SENSIRION_START_TEMP_MEASUREMENT                0x1001
#define SENSIRION_READ_SCALE_FACTOR                     0x30DE
#define SENSIRION_READ_OFFSET                           0x30DF
#define SENSIRION_READ_ARTICLE_NUMBER_P1                0x31E3
#define SENSIRION_READ_ARTICLE_NUMBER_P2                0x31E4
#define SENSIRION_READ_SERIAL_NUMBER_P1                 0x31AE
#define SENSIRION_READ_SERIAL_NUMBER_P2                 0x31AF
#define SENSIRION_SOFTWARE_RESET                        0x2000

#define VOLUME_SETPOINT_TOLERANCE_ML                    5
#define VOLUME_MIN_ACCUMULATED_VALUE_ML                 50

#define FLOW_THRESHOLD_INDEX_BY_DEFAULT					0

// ----------------------- Defines for digital filters -------------------------

#define FLOW_LPF_FC_HZ                  1.0
#define FIO2_LPF_FC_HZ                  1.0
#define BATT_LPF_FC_HZ                  0.01//0.1

#define FLOW_LPF_RC_CONSTANT_S            (1 / (6.28 * FLOW_LPF_FC_HZ))
#define FIO2_LPF_RC_CONSTANT_S            (1 / (6.28 * FIO2_LPF_FC_HZ))
#define BATT_LPF_RC_CONSTANT_S            (1 / (6.28 * BATT_LPF_FC_HZ))

#define FLOW_SENSOR_SAMPLE_PERIOD_MS        10
#define FIO2_SENSOR_SAMPLE_PERIOD_MS        10
#define BATT_SENSOR_SAMPLE_PERIOD_MS		10

// ---------------------- Defines for the oxygen sensor ------------------------

#define FIO2_SENSOR_MIN_OUT_V   0.000230349
#define FIO2_SENSOR_MAX_OUT_V   0.05291946
#define FIO2_AMP_GAIN           10.0

#define FIO2_CALIB_LINE_SLOPE_BY_DEFAULT		1897.92536
#define FIO2_CALIBE_LINE_OFFSET_BY_DEFAULT		-0.43718521

#define FIO2_SENSOR_ERROR_THRESHOLD_V			0.00035

#define FIO2_THRESHOLD_INDEX_BY_DEFAULT			0

// ---------------------- Defines for battery indicator ------------------------

//#define BATT_LEVEL_THRESHOLD	5

#define BATT_MIN_OUT_V					2.75
#define BATT_MAX_OUT_V					4.2
#define BATT_AMP_GAIN					0.25

#define BATT_CHARGE_THRESHOLD_V			4.5
#define BATT_CHARGE_GAUGE_STATES		4

#define N_BATT_LEVELS					10
#define BATT_FULL_RANGE					(BATT_MAX_OUT_V - BATT_MIN_OUT_V)
#define BATT_MEASURE_STEP				(BATT_FULL_RANGE / N_BATT_LEVELS)

#define BATT_BASE_LEVEL_0				BATT_MIN_OUT_V
#define BATT_BASE_LEVEL_1				(BATT_MIN_OUT_V + BATT_MEASURE_STEP)
#define BATT_BASE_LEVEL_2				(BATT_MIN_OUT_V + (2*BATT_MEASURE_STEP))
#define BATT_BASE_LEVEL_3				(BATT_MIN_OUT_V + (3*BATT_MEASURE_STEP))
#define BATT_BASE_LEVEL_4				(BATT_MIN_OUT_V + (4*BATT_MEASURE_STEP))
#define BATT_BASE_LEVEL_5				(BATT_MIN_OUT_V + (5*BATT_MEASURE_STEP))
#define BATT_BASE_LEVEL_6				(BATT_MIN_OUT_V + (6*BATT_MEASURE_STEP))
#define BATT_BASE_LEVEL_7				(BATT_MIN_OUT_V + (7*BATT_MEASURE_STEP))
#define BATT_BASE_LEVEL_8				(BATT_MIN_OUT_V + (8*BATT_MEASURE_STEP))
#define BATT_BASE_LEVEL_9				(BATT_MIN_OUT_V + (9*BATT_MEASURE_STEP))
#define BATT_BASE_LEVEL_10				(BATT_MIN_OUT_V + (10*BATT_MEASURE_STEP))

#define BATT_LEVEL_THRESHOLD			(BATT_MEASURE_STEP / 4.0) // level threshold = 0.4

#define BATT_LEVEL_00_LOW_THRESHOLD		(BATT_BASE_LEVEL_0 + BATT_LEVEL_THRESHOLD)
#define BATT_LEVEL_00_HIGH_THRESHOLD	(BATT_BASE_LEVEL_1 - BATT_LEVEL_THRESHOLD)

#define BATT_LEVEL_10_LOW_THRESHOLD		(BATT_BASE_LEVEL_1 + BATT_LEVEL_THRESHOLD)
#define BATT_LEVEL_10_HIGH_THRESHOLD	(BATT_BASE_LEVEL_2 - BATT_LEVEL_THRESHOLD)

#define BATT_LEVEL_20_LOW_THRESHOLD		(BATT_BASE_LEVEL_2 + BATT_LEVEL_THRESHOLD)
#define BATT_LEVEL_20_HIGH_THRESHOLD	(BATT_BASE_LEVEL_3 - BATT_LEVEL_THRESHOLD)

#define BATT_LEVEL_30_LOW_THRESHOLD		(BATT_BASE_LEVEL_3 + BATT_LEVEL_THRESHOLD)
#define BATT_LEVEL_30_HIGH_THRESHOLD	(BATT_BASE_LEVEL_4 - BATT_LEVEL_THRESHOLD)

#define BATT_LEVEL_40_LOW_THRESHOLD		(BATT_BASE_LEVEL_4 + BATT_LEVEL_THRESHOLD)
#define BATT_LEVEL_40_HIGH_THRESHOLD	(BATT_BASE_LEVEL_5 - BATT_LEVEL_THRESHOLD)

#define BATT_LEVEL_50_LOW_THRESHOLD		(BATT_BASE_LEVEL_5 + BATT_LEVEL_THRESHOLD)
#define BATT_LEVEL_50_HIGH_THRESHOLD	(BATT_BASE_LEVEL_6 - BATT_LEVEL_THRESHOLD)

#define BATT_LEVEL_60_LOW_THRESHOLD		(BATT_BASE_LEVEL_6 + BATT_LEVEL_THRESHOLD)
#define BATT_LEVEL_60_HIGH_THRESHOLD	(BATT_BASE_LEVEL_7 - BATT_LEVEL_THRESHOLD)

#define BATT_LEVEL_70_LOW_THRESHOLD		(BATT_BASE_LEVEL_7 + BATT_LEVEL_THRESHOLD)
#define BATT_LEVEL_70_HIGH_THRESHOLD	(BATT_BASE_LEVEL_8 - BATT_LEVEL_THRESHOLD)

#define BATT_LEVEL_80_LOW_THRESHOLD		(BATT_BASE_LEVEL_8 + BATT_LEVEL_THRESHOLD)
#define BATT_LEVEL_80_HIGH_THRESHOLD	(BATT_BASE_LEVEL_9 - BATT_LEVEL_THRESHOLD)

#define BATT_LEVEL_90_LOW_THRESHOLD		(BATT_BASE_LEVEL_9 + BATT_LEVEL_THRESHOLD)
#define BATT_LEVEL_90_HIGH_THRESHOLD	(BATT_BASE_LEVEL_10 - BATT_LEVEL_THRESHOLD)

#define BATT_LEVEL_100_LOW_THRESHOLD	(BATT_BASE_LEVEL_10 + BATT_LEVEL_THRESHOLD)
#define BATT_LEVEL_100_HIGH_THRESHOLD	(BATT_BASE_LEVEL_10 + BATT_LEVEL_THRESHOLD)	

// ------------------------- Defines for the alarm -----------------------------

#define FLOW_THRESHOLD_ADJUST_STATE		0
#define FIO2_THRESHOLD_ADJUST_STATE		1

#define FLOW_THRESHOLD_N_VALUES			5
#define FIO2_THRESHOLD_N_VALUES			5

// ------------------------ Defines for calibration ----------------------------

#define N_CALIBRATION_REFERENCE_POINTS			8
#define N_CALIBRATION_SAMPLES_PER_POINT			4
#define N_CALIBRATION_TOTAL_SAMPLES				(N_CALIBRATION_REFERENCE_POINTS*N_CALIBRATION_SAMPLES_PER_POINT)

#define CALIB_UPPER_BOUNDARY_01					29
#define CALIB_UPPER_BOUNDARY_02					39
#define CALIB_UPPER_BOUNDARY_03					49
#define CALIB_UPPER_BOUNDARY_04					59
#define CALIB_UPPER_BOUNDARY_05					69
#define CALIB_UPPER_BOUNDARY_06					79
#define CALIB_UPPER_BOUNDARY_07					89
#define CALIB_UPPER_BOUNDARY_08					99

#define CALIB_LOWER_BOUNDARY_01					20
#define CALIB_LOWER_BOUNDARY_02					30
#define CALIB_LOWER_BOUNDARY_03					40
#define CALIB_LOWER_BOUNDARY_04					50
#define CALIB_LOWER_BOUNDARY_05					60
#define CALIB_LOWER_BOUNDARY_06					70
#define CALIB_LOWER_BOUNDARY_07					80
#define CALIB_LOWER_BOUNDARY_08					90

// ----------------------------- System flags ----------------------------------

#define READ_SENSORS_FLAG					0		
#define FIO2_SENSOR_ERROR_FLAG				1
#define FLOW_SENSOR_ERROR_FLAG				2
#define DISPLAY_UPDATE_FLAG					3
#define ALARM_ENABLED_FLAG					4
#define FLOW_ALARM_FLAG						5
#define FIO2_ALARM_FLAG						6
#define ALARM_CONFIG_MODE_FLAG				7
#define SYSTEM_SHUT_DOWN_FLAG				8
#define BATTERY_CHARGING_FLAG				9
#define BATTERY_CHARGE_GAUGE_FLAG			10
#define CALIBRATION_MODE_FLAG				11
#define CALIBRATION_SAMPLE_COMPLETE_FLAG	12
#define CALIBRATION_ERROR_FLAG				13

#define SENSOR_ERROR_MASK		((1 << FIO2_SENSOR_ERROR_FLAG) | (1 << FLOW_SENSOR_ERROR_FLAG))
#define BOUNDARIES_ERROR_MASK	((1 << FLOW_ALARM_FLAG	) | (1 << FIO2_ALARM_FLAG))

// ---------------------- Reading multiple buttons bushed at the same time ---------------------------

#define ALARM_ON_OFF_BUTTON_PRESSED		(!(PIN_ALARM_ON_OFF_BUTTON & (1 << ALARM_ON_OFF_BUTTON)))
#define SYSTEM_MENU_BUTTON_PRESSED		(!(PIN_SYSTEM_MENU_BUTTON & (1 << SYSTEM_MENU_BUTTON)))
#define TWO_BUTTONS_PRESSED				(ALARM_ON_OFF_BUTTON_PRESSED && SYSTEM_MENU_BUTTON_PRESSED)

// ----------------------------- Defines for first time programming ----------------------------------

#define FIRST_TIME_PROGRAMMING		0


#endif