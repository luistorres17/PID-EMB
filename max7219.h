#ifndef MAX7219_H
#define MAX7219_H

#include <stdint.h>
#include "tm4c1294ncpdt.h"

// --- MAPA DE REGISTROS MAX7219 ---
// Direcciones internas del controlador de display
#define MAX7219_REG_NOOP        0x00 // No Operación
#define MAX7219_REG_DIGIT0      0x01 // Dígito 0 (Derecha)
#define MAX7219_REG_DIGIT1      0x02
#define MAX7219_REG_DIGIT2      0x03
#define MAX7219_REG_DIGIT3      0x04
#define MAX7219_REG_DIGIT4      0x05
#define MAX7219_REG_DIGIT5      0x06
#define MAX7219_REG_DIGIT6      0x07
#define MAX7219_REG_DIGIT7      0x08 // Dígito 7 (Izquierda)
#define MAX7219_REG_DECODEMODE  0x09 // Modo de decodificación (BCD vs Directo)
#define MAX7219_REG_INTENSITY   0x0A // Control de brillo
#define MAX7219_REG_SCANLIMIT   0x0B // Límite de escaneo (número de dígitos activos)
#define MAX7219_REG_SHUTDOWN    0x0C // Registro de apagado/encendido
#define MAX7219_REG_DISPLAYTEST 0x0F // Registro de prueba de segmentos

// --- PROTOTIPOS DE FUNCIONES ---

// Configuración inicial del protocolo y registros
void MAX7219_Init(void);

// Envío de datos a bajo nivel (Registro, Valor)
void MAX7219_Send(uint8_t registro, uint8_t dato);

// Borrado completo de la pantalla
void MAX7219_Clear(void);

// Ajuste dinámico de la intensidad
void MAX7219_SetIntensity(uint8_t intensidad);

// Función de interfaz de usuario: 
// Visualiza simultáneamente la salida de control (Izq) y la realimentación (Der)
void MAX7219_ShowStatus(uint32_t valor_pwm, uint32_t valor_encoder);

#endif