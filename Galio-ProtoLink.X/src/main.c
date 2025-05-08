/* 
 * File:   main.c
 * Author: Arturo Gasca
 *
 * Created on 4 de febrero de 2025, 04:11 PM
 */

#include <18F67K40.h>
#device ADC = 10
//#use fast_io(G)
#include "tunacore.h"

#define DEVICE_DEBUG            TRUE
#define DEVICE_SERIAL_SOURCE    UART_INT_RDA3
#define DEVICE_SERIAL_BAUD      9600
#define DEVICE_RST_PIN          COM_RST
#include <string.h>
#include <stdlib.h>
#include <stdlibm.h>
#include <ieeefloat.c>




/*Hardware defines*/
#define COMPILACION_DATE    __DATE__
#define COMPILACION_TIME    __TIME__
#define FW_VERSION  1.0
#define HW_NAME "Galio Modbus Slave Demo"

#fuses HS
#use delay(crystal=20mhz)

#use rs232(baud=115200,parity=N,UART1,bits=8,stream=DEBUG,errors)
//#use rs232(baud=115200,parity=N,UART3,bits=8,stream=LAN,errors)
//#use rs232(baud=9600,parity=N,UART2,bits=8,stream=DISPLAY,errors)
//#use rs232(baud=9600,parity=N,UART5,bits=8,stream=MAX,errors)

#define DEBUG_MSG(msg) fprintf(DEBUG, msg)
#define DEBUG_DATA(msg,data) fprintf(DEBUG,msg,data)


//MODBUS DEFINES
#define MODBUS_PROTOCOL             MODBUS_PROTOCOL_SERIAL
#define MODBUS_TYPE                 MODBUS_TYPE_SLAVE
#define MODBUS_SERIAL_TYPE          MODBUS_RTU     //use MODBUS_ASCII for ASCII mode
#define MODBUS_SERIAL_RX_BUFFER_SIZE 64
#define MODBUS_SERIAL_BAUD          115200
#define MODBUS_PARITY               "NONE"
#define MODBUS_SERIAL_INT_SOURCE    MODBUS_INT_RDA5
#define MODBUS_SERIAL_RX_PIN        RX_MODBUS
#define MODBUS_SERIAL_TX_PIN        TX_MODBUS
#define MODBUS_SERIAL_ENABLE_PIN    TX_ENABLE
#define MODBUS_SERIAL_RX_ENABLE     RX_ENABLE
//#define MODBUS_SERIAL_TIMEOUT       100000


//#include <bootloader.h>
#include "tuna-modbus/modbus.c"
#include "devices/generic_uart.h"

#define MODBUS_ADDRESS 0x01
#define DELAY_LED   1000



// Buffer para recibir la trama (ajustado al tamaño típico)
//char trama[16];
//int trama_index = 0;

int8 coils = 0b00000101;
int8 inputs = 0b00001001;
//int16 hold_regs[] = {0x8800,0x7700,0x6600,0x5500,0x4400,0x3300,0x2200,0x1100};
//int16 input_regs[] = {0x1100,0x2200,0x3300,0x4400,0x5500,0x6600,0x7700,0x8800};
int16 event_count = 0;
int16 input_registers[10];
int16 holding_registers[10];
// Variables extraídas
long long peso_int32 = 0;
long peso_hi, peso_lo;
float peso = 0.0;
char unidad[3] = {0}; // 2 letras + terminador
char estado = 0;

// Función para procesar la trama completa

void procesar_trama() {
    char peso_str[8]; // Para "+0123.4"
    //Para indicador ricelake 120
    if (data.buffer[0] == 0x02) { // Verifica que inicie con STX
        // Copiar peso
        strncpy(peso_str, &data.buffer[1], 7);
        peso_str[7] = '\0'; // Fin de cadena

        // Copiar unidad
        unidad[0] = data.buffer[8];
        unidad[1] = data.buffer[9];
        unidad[2] = '\0';

        // Copiar estado
        estado = data.buffer[10];

        // Convertir peso
        peso = atof(peso_str);


        // Ahora tienes el peso como float, unidad y estado
        //fprintf(DEBUG,"Peso: %.2f %s\n", peso, unidad);
        //fprintf(DEBUG,"Estado: %c\n", estado);
    }//Para indicador YP200
    else if (data.buffer[0] == 0x3D) {
        strncpy(peso_str, data.buffer + 1, 7);
        peso_str[7] = '\0';
        peso = atof(peso_str);
        //fprintf(DEBUG, "Peso: %.2f \n",peso);
    } else {
        //DEBUG_MSG("WRONG PROTOCOL");
    }
    output_toggle(LED2);
}








//int16 value = 0;

int8 swap_bits(int8 c) {
    return ((c & 1) ? 128 : 0) | ((c & 2) ? 64 : 0) | ((c & 4) ? 32 : 0) | ((c & 8) ? 16 : 0) | ((c & 16) ? 8 : 0)
            | ((c & 32) ? 4 : 0) | ((c & 64) ? 2 : 0) | ((c & 128) ? 1 : 0);
}

void procesar_modbus(void) {
    //check address against our address, 0 is broadcast
    if ((modbus_rx.address == MODBUS_ADDRESS) || modbus_rx.address == 0) {
        //output_high(LED1);
        //delay_ms(10);
        output_toggle(LED1);
        switch (modbus_rx.func) {
            case FUNC_READ_COILS: //read coils
                //DEBUG_MSG("[DEBUG] FUNC_READ_COILS \r\n");
                break;
            case FUNC_READ_DISCRETE_INPUT: //read inputs
                //DEBUG_MSG("[DEBUG] FUNC_READ_DISCRETE_INPUT \r\n");
                if (modbus_rx.data[0] || modbus_rx.data[2] ||
                        modbus_rx.data[1] >= 8 || modbus_rx.data[3] + modbus_rx.data[1] > 8)
                    modbus_exception_rsp(MODBUS_ADDRESS, modbus_rx.func, ILLEGAL_DATA_ADDRESS);
                else {
                    int8 data;

                    if (modbus_rx.func == FUNC_READ_COILS)
                        data = coils >> (modbus_rx.data[1]); //move to the starting coil
                    else
                        data = inputs >> (modbus_rx.data[1]); //move to the starting input

                    data = data & (0xFF >> (8 - modbus_rx.data[3])); //0 out values after quantity

                    if (modbus_rx.func == FUNC_READ_COILS)
                        modbus_read_coils_rsp(MODBUS_ADDRESS, 0x01, &data);
                    else
                        modbus_read_discrete_input_rsp(MODBUS_ADDRESS, 0x01, &data);

                    event_count++;
                }
                break;
            case FUNC_READ_HOLDING_REGISTERS:
            case FUNC_READ_INPUT_REGISTERS:

                if (modbus_rx.data[0] || modbus_rx.data[2] ||
                        modbus_rx.data[1] >= 8 || modbus_rx.data[3] + modbus_rx.data[1] > 8) {
                    modbus_exception_rsp(MODBUS_ADDRESS, modbus_rx.func, ILLEGAL_DATA_ADDRESS);
                } else {
                    if (modbus_rx.func == FUNC_READ_HOLDING_REGISTERS) {
                        // DEBUG_MSG("[DEBUG] FUNC_READ_HOLDING_REGISTERS \r\n");
                        modbus_read_holding_registers_rsp(MODBUS_ADDRESS, (modbus_rx.data[3]*2), holding_registers + modbus_rx.data[1]);
                    } else {
                        // DEBUG_MSG("[DEBUG] FUNC_READ_INPUT_REGISTERS \r\n");
                        modbus_read_input_registers_rsp(MODBUS_ADDRESS, (modbus_rx.data[3]*2), input_registers + modbus_rx.data[1]);
                    }

                    event_count++;
                    //output_low(LED1);
                }
                break;
            case FUNC_WRITE_SINGLE_COIL: //write coil
                //DEBUG_MSG("[DEBUG] FUNC_WRITE_SINGLE_COIL \r\n");
                if (modbus_rx.data[0] || modbus_rx.data[3] || modbus_rx.data[1] > 8)
                    modbus_exception_rsp(MODBUS_ADDRESS, modbus_rx.func, ILLEGAL_DATA_ADDRESS);
                else if (modbus_rx.data[2] != 0xFF && modbus_rx.data[2] != 0x00)
                    modbus_exception_rsp(MODBUS_ADDRESS, modbus_rx.func, ILLEGAL_DATA_VALUE);
                else {
                    if (modbus_rx.data[2] == 0xFF)
                        bit_set(coils, modbus_rx.data[1]);
                    else
                        bit_clear(coils, modbus_rx.data[1]);

                    modbus_write_single_coil_rsp(MODBUS_ADDRESS, modbus_rx.data[1], ((int16) (modbus_rx.data[2])) << 8);

                    event_count++;
                }
                break;
            case FUNC_WRITE_SINGLE_REGISTER:
                //DEBUG_MSG("[DEBUG] FUNC_WRITE_SINGLE_REGISTER \r\n");
                if (modbus_rx.data[0] || modbus_rx.data[1] >= 8)
                    modbus_exception_rsp(MODBUS_ADDRESS, modbus_rx.func, ILLEGAL_DATA_ADDRESS);
                else {
                    holding_registers[modbus_rx.data[1]] = make16(modbus_rx.data[2], modbus_rx.data[3]);

                    modbus_write_single_register_rsp(MODBUS_ADDRESS,
                            make16(modbus_rx.data[0], modbus_rx.data[1]),
                            make16(modbus_rx.data[2], modbus_rx.data[3]));
                }
                break;
            case FUNC_WRITE_MULTIPLE_COILS:
                //DEBUG_MSG("[DEBUG] FUNC_WRITE_MULTIPLE_COILS \r\n");
                if (modbus_rx.data[0] || modbus_rx.data[2] ||
                        modbus_rx.data[1] >= 8 || modbus_rx.data[3] + modbus_rx.data[1] > 8)
                    modbus_exception_rsp(MODBUS_ADDRESS, modbus_rx.func, ILLEGAL_DATA_ADDRESS);
                else {
                    int i, j;

                    modbus_rx.data[5] = swap_bits(modbus_rx.data[5]);

                    for (i = modbus_rx.data[1], j = 0; i < modbus_rx.data[1] + modbus_rx.data[3]; ++i, ++j) {
                        if (bit_test(modbus_rx.data[5], j))
                            bit_set(coils, 7 - i);
                        else
                            bit_clear(coils, 7 - i);
                    }

                    modbus_write_multiple_coils_rsp(MODBUS_ADDRESS,
                            make16(modbus_rx.data[0], modbus_rx.data[1]),
                            make16(modbus_rx.data[2], modbus_rx.data[3]));

                    event_count++;
                }
                break;
            case FUNC_WRITE_MULTIPLE_REGISTERS:
                // DEBUG_MSG("[DEBUG] FUNC_WRITE_MULTIPLE_REGISTERS \r\n");
                if (modbus_rx.data[0] || modbus_rx.data[2] ||
                        modbus_rx.data[1] >= 8 || modbus_rx.data[3] + modbus_rx.data[1] > 8)
                    modbus_exception_rsp(MODBUS_ADDRESS, modbus_rx.func, ILLEGAL_DATA_ADDRESS);
                else {
                    int i, j;

                    for (i = 0, j = 5; i < modbus_rx.data[4] / 2; ++i, j += 2)
                        holding_registers[i] = make16(modbus_rx.data[j], modbus_rx.data[j + 1]);

                    modbus_write_multiple_registers_rsp(MODBUS_ADDRESS,
                            make16(modbus_rx.data[0], modbus_rx.data[1]),
                            make16(modbus_rx.data[2], modbus_rx.data[3]));

                    event_count++;
                }
                break;
            default: //We don't support the function, so return exception
                //DEBUG_MSG("[DEBUG] We don´t Support the function \r\n");
                modbus_exception_rsp(MODBUS_ADDRESS, modbus_rx.func, ILLEGAL_FUNCTION);
        }
    }

}

void main() {
    //setup_oscillator(OSC_HFINTRC_64MHZ); 

    set_tris_a(0b00000001); //Pongo el RA0 como entrada
    set_tris_g(0b00000100);
    setup_adc_ports(sAN0); //Pongo todo el puerto a analogo
    setup_adc(ADC_CLOCK_INTERNAL); //Selecciono reloj interno para conversion




    int i;
    for (i = 0; i < 10; i++) {
        input_registers[i] = 0x0000;
        holding_registers[i] = 0x0000;

    }



    modbus_init();
    device_init();
    enable_interrupts(GLOBAL);
    // si funciona 
    //peso = 999999.0;
    //peso_int32 = peso; 
    // peso_hi = (int16)(peso_int32 >> 16);       // Parte alta (bits 31?16)
    // peso_lo = (int16)(peso_int32 & 0xFFFF);    // Parte baja (bits 15?0)
    //input_registers[0] = peso_hi;
    // input_registers[1] = peso_lo;

    //output_low(TX_ENABLE);


    while (TRUE) {
        if (data.AVAILABLE) {
            data.AVAILABLE = false;
            procesar_trama();
            peso_int32 = f_PICtoIEEE(peso);
            peso_hi = (int16) (peso_int32 >> 16); // Parte alta (bits 31?16)
            peso_lo = (int16) (peso_int32 & 0xFFFF); // Parte baja (bits 15?0)
            input_registers[0] = peso_hi;
            input_registers[1] = peso_lo;
        }
        if (modbus_kbhit()) {
            procesar_modbus();

        }
    }
}