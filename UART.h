#ifndef UART_H
#define UART_H
#include <stdint.h>
#include "tm4c1294ncpdt.h"



//prototipos

void Config_UART0_115200(void);
void UART0_SendChar(char data);
void UART0_PrintString(char *str);
void UART0_PrintInt(uint32_t number);


#endif