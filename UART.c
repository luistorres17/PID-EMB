#include "UART.h"


void Config_UART0_115200(void) {
    // Configuración del puerto serie para enviar datos a la PC
    SYSCTL_RCGCUART_R |= 0x01; 
    SYSCTL_RCGCGPIO_R |= 0x01;
    while((SYSCTL_PRGPIO_R & 0x01) == 0){};
    
    // Pines PA0 (RX) y PA1 (TX)
    GPIO_PORTA_AFSEL_R |= 0x03; 
    GPIO_PORTA_PCTL_R = (GPIO_PORTA_PCTL_R & 0xFFFFFF00) | 0x00000011; 
    GPIO_PORTA_DEN_R   |= 0x03;
    
    // Configurar baudrate a 115200 (para reloj de 120MHz)
    UART0_CTL_R &= ~0x00000001; // Deshabilitar
    UART0_IBRD_R = 65; // Parte entera
    UART0_FBRD_R = 7;  // Parte fraccionaria
    UART0_LCRH_R = 0x00000070; // 8 bits, sin paridad, FIFO activado
    UART0_CTL_R |= 0x00000301; // Habilitar TX, RX y módulo UART
}

// Funciones auxiliares para enviar texto y números
void UART0_SendChar(char data) {
    while((UART0_FR_R & 0x00000020) != 0){}; // Esperar si está lleno
    UART0_DR_R = data;
}
void UART0_PrintString(char *str) {
    while(*str) { UART0_SendChar(*str++); }
}
void UART0_PrintInt(uint32_t number) {
    char buffer[16];
    int i = 0;
    if (number == 0) { UART0_SendChar('0'); return; }
    // Convertir número a cadena (invertido)
    while (number > 0) {
        buffer[i++] = (number % 10) + '0';
        number /= 10;
    }
    // Enviar caracteres en orden correcto
    while (i > 0) { UART0_SendChar(buffer[--i]); }
}