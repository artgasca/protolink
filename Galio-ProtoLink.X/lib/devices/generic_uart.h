/* 
 * File:   generic_uart.h
 * Author: artga
 *
 * Created on 12 de enero de 2022, 12:20 PM
 */

#ifndef GENERIC_UART_H
#define	GENERIC_UART_H

#define UART_INT_RDA     	11111
#define UART_INT_RDA2		22222
#define UART_INT_RDA3    	33333
#define UART_INT_RDA4    	44444
#define UART_INT_RDA5       55555  

#define BUFFER_SIZE 255
#ifndef DEVICE_DEBUG
#define DEVICE_DEBUG FALSE
#endif

struct data_uart{
    char buffer[BUFFER_SIZE];
    BOOLEAN AVAILABLE;
    long len;
        
}data;
/*Generic functions*/
void device_init(void);
BOOLEAN device_reset(BOOLEAN pulse, long time_ms);
void data_send(char* buffer, long len);
BOOLEAN data_timeout(long millis);

#include "generic_uart.c"

#endif	/* GENERIC_UART_H */

