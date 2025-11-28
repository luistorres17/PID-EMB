/* config.h - Cabecera de Configuración de Periféricos */
#ifndef CONFIG_H
#define CONFIG_H

#include <stdint.h>
#include "tm4c1294ncpdt.h"

// --- VARIABLES GLOBALES ---
// Variable volátil para acceso compartido entre interrupciones y programa principal
extern volatile uint32_t g_CicloTrabajo; 

// --- PROTOTIPOS DE FUNCIONES ---

// Sistema y Reloj
void Config_FPU(void);          // Habilitar Unidad de Punto Flotante
void Delay_ms(uint32_t ms);     // Retardo por software

// Actuador (PWM)
void Config_PWM(void);             // Inicializar módulo PWM0
void PWM_SetDuty(uint32_t duty);   // Establecer ciclo de trabajo (0-100%)

// Sensor (Encoder de Cuadratura)
void Config_QEI(void);             // Inicializar interfaz QEI
uint32_t QEI_GetPosition(void);    // Leer contador de posición absoluta
uint32_t QEI_GetVelocity(void);    // Leer estimación de velocidad
int32_t QEI_GetDirection(void);    // Leer dirección de giro (+1/-1)

// Comunicación Serie (UART/Debug)
void Config_UART0_115200(void);      // Configurar UART0 para telemetría
void UART0_PrintString(char *cadena);// Enviar texto
void UART0_PrintInt(uint32_t numero);// Enviar número entero
void UART0_SendChar(char caracter);  // Enviar un solo byte

// Interfaz Periférica Serie (SPI/SSI2) para Display
void Config_SSI2_Display(void);
void SPI_WriteByte(uint8_t dato); // Transmisión directa de byte
void CS_Select(void);             // Activación de periférico (Bajar línea)
void CS_Deselect(void);           // Desactivación de periférico (Subir línea)

#endif // CONFIG_H