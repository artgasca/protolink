#include "generic_uart.h"

#if (DEVICE_SERIAL_SOURCE == UART_INT_RDA)
#use rs232(baud=DEVICE_SERIAL_BAUD,parity=N,UART1,bits=8,stream=UART_PORT,restart_wdt,errors)
#elif (DEVICE_SERIAL_SOURCE == UART_INT_RDA2)
#use rs232(baud=DEVICE_SERIAL_BAUD,parity=N,UART2,bits=8,stream=UART_PORT,restart_wdt,errors)
#elif (DEVICE_SERIAL_SOURCE == UART_INT_RDA3)
#use rs232(baud=DEVICE_SERIAL_BAUD,parity=N,UART3,bits=8,stream=UART_PORT,restart_wdt,errors)
#elif (DEVICE_SERIAL_SOURCE == UART_INT_RDA4)
#use rs232(baud=DEVICE_SERIAL_BAUD,parity=N,UART4,bits=8,stream=UART_PORT,restart_wdt,errors)
#elif (DEVICE_SERIAL_SOURCE == UART_INT_RDA5)
#use rs232(baud=DEVICE_SERIAL_BAUD,parity=N,UART5,bits=8,stream=UART_PORT,restart_wdt,errors)
#endif


#if (DEVICE_SERIAL_SOURCE == UART_INT_RDA)
#int_rda
#elif (DEVICE_SERIAL_SOURCE == UART_INT_RDA2)
#int_rda2
#elif (DEVICE_SERIAL_SOURCE == UART_INT_RDA3)
#int_rda3
#elif (DEVICE_SERIAL_SOURCE == UART_INT_RDA4)
#int_rda4
#elif (DEVICE_SERIAL_SOURCE == UART_INT_RDA5)
#int_rda5
#endif
void uart_serial_rcv(){
    static int16 replyidx = 0;
    char c = fgetc(UART_PORT);
    if (replyidx < BUFFER_SIZE) {
        data.buffer[replyidx++] = c;

    }

    
    if(c == '\n' || replyidx >=10){
        data.buffer[replyidx] = 0;
        data.len = replyidx;
        data.AVAILABLE = TRUE;
        replyidx = 0;
    }
}
/*void uart_serial_rcv(){
    int16 timeout = 500;
    int16 replyidx = 0;
    int16 timeoutBack = timeout;
    while(timeout--){
        if(replyidx >= BUFFER_SIZE){
            break;
        }
        if(kbhit(UART_PORT)){
            timeout = timeoutBack;
            char c = fgetc(UART_PORT);            
            data.buffer[replyidx] = c;
            replyidx++;
        }
        if(timeout == 0){
            break;
        }
    }
    data.buffer[replyidx] = 0; //null term
    data.AVAILABLE = TRUE;
    data.len = replyidx;
}*/

void clean_buffer(void){
    memset(data.buffer,0,sizeof(data.buffer));
}

void device_init(void){
    
    data.AVAILABLE = FALSE;
    data.len = 0;
    clean_buffer();
#if (DEVICE_SERIAL_SOURCE==UART_INT_RDA)
    clear_interrupt(INT_RDA);
    enable_interrupts(INT_RDA);
#elif (DEVICE_SERIAL_SOURCE==UART_INT_RDA2)
    clear_interrupt(INT_RDA2);
    enable_interrupts(INT_RDA2);
#elif (DEVICE_SERIAL_SOURCE==UART_INT_RDA3)
    clear_interrupt(INT_RDA3);
    enable_interrupts(INT_RDA3);
#elif (DEVICE_SERIAL_SOURCE==UART_INT_RDA4)
    clear_interrupt(INT_RDA4);
    enable_interrupts(INT_RDA4);
#elif (DEVICE_SERIAL_SOURCE==UART_INT_RDA5)
   clear_interrupt(INT_RDA5);
   enable_interrupts(INT_RDA5);
#endif
}
BOOLEAN device_reset(BOOLEAN pulse, long time_ms){
    if (pulse) {
        output_high(DEVICE_RST_PIN);
        delay_ms(time_ms);
        output_low(DEVICE_RST_PIN);
        while(input(DEVICE_RST_PIN)){
            DEBUG_MSG("1 En RST \r\n");
        }

    }else{
        output_low(DEVICE_RST_PIN);
        delay_ms(time_ms);
        output_high(DEVICE_RST_PIN);
        while(!input(DEVICE_RST_PIN)){
            DEBUG_MSG("0 En RST\r\n");
        }
        
    }
    return true;
    

}

void data_send(char* buffer, long len){
    long i;
    for (i = 0; i < len; i++) {
        fputc(buffer[i],UART_PORT);
    }
#if (DEVICE_DEBUG == TRUE)
    DEBUG_MSG("[DEBUG]Data sent: [");
    for (i = 0; i < len; i++) {
        DEBUG_DATA("%X:",buffer[i]);
    }
    DEBUG_MSG("]\n");
#endif
    

}

BOOLEAN data_timeout(long millis){
    int16 timeout_ms = 0;
    while (!data.AVAILABLE && millis > timeout_ms) {
        timeout_ms++;
        delay_ms(1);
    }
    

    if (data.AVAILABLE) {
#if (DEVICE_DEBUG == TRUE) 
        DEBUG_MSG("[DEVICE] Data available\r\n");
#endif
        return FALSE;
    }
    else if(timeout_ms >= millis){
#if (DEVICE_DEBUG == TRUE) 
        DEBUG_MSG("[DEVICE] Timeout Serial\r\n");
#endif
        return TRUE;
    }

}