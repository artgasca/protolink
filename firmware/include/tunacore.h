/* 
 * File:   tunacore.h
 * Author: Arturo Gasca
 *
 * Modified on 04 de Agosto de 2021
 */

#ifndef TUNACORE_H
#define	TUNACORE_H

/*USER BUTTON AND LEDS*/
#define USER        PIN_B2  //Interruption 2
#define LED1        PIN_F0
#define LED2        PIN_F1

/*USB SERIAL*/
#define TX_1        PIN_C6
#define RX_1        PIN_C7

/*HEADER RS485*/
#define TX_5        PIN_E2
#define RX_5        PIN_E3
#define TX_ENABLE   PIN_E6  //Internal Auxiliar
#define RX_ENABLE   PIN_E7  //Internal Auxiliar

/*HEADER UART*/
#define TX_4        PIN_C0
#define RX_4        PIN_C1

/*HEADER RS232*/
#define TX_2        PIN_G1
#define RX_2        PIN_G2

/*HEADER ANALOG*/
#define ANALOG_1    PIN_A0
#define ANALOG_2    PIN_A1

/*HEADER PWM*/
#define PWM_1       PIN_E5
#define PWM_2       PIN_E4

/*EEPROM Write Protect*/
#define EE_WP       PIN_G7

/*RTC AUX*/
#define RTC_INT     PIN_B3  //Interruption 3

/*ACCESORY EXPANSION HEADER*/
#define EXP_SDA     PIN_C4  //I2C General Board
#define EXP_SCL     PIN_C3  //I2C General Board
#define EXP_GPIO1   PIN_F2
#define EXP_GPIO2   PIN_F3
#define EXP_INT0    PIN_B0
#define EXP_INT1    PIN_B1

/*TELEMETRY COMMUNICATION HEADER*/
#define COM_TX      PIN_E0 //TX UART3
#define COM_RX      PIN_E1 //RX UART3
#define COM_MISO    PIN_D4
#define COM_MOSI    PIN_D5
#define COM_SCLK    PIN_D6
#define COM_AUX     PIN_D7
#define COM_RST     PIN_G0



//UART1
#pin_select U1TX=TX_1
#pin_select U1RX=RX_1

//UART2
#pin_select U2TX=TX_2
#pin_select U2RX=RX_2

//UART3
#pin_select U3TX=COM_TX
#pin_select U3RX=COM_RX

//UART4
#pin_select U4TX=TX_4
#pin_select U4RX=RX_4

//UART5
#pin_select U5TX=TX_5
#pin_select U5RX=RX_5

//I2C1
#pin_select SCL1IN=EXP_SCL
#pin_select SDA1IN=EXP_SDA



#endif	/* TUNACORE_H */

