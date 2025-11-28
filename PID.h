/* pid.h - Definición de la estructura de Control PID */
#ifndef PID_H
#define PID_H

#include <stdint.h>

// Estructura contenedora del objeto controlador
typedef struct {
    // --- Parámetros de Sintonización (Ganancias) ---
    float Kp;  // Ganancia Proporcional: Respuesta al error presente
    float Ki;  // Ganancia Integral: Corrección del error acumulado (histórico)
    float Kd;  // Ganancia Derivativa: Predicción basada en la tasa de cambio

    // --- Limitaciones Físicas del Actuador ---
    float limMin; // Salida mínima (ej. Zona muerta del motor)
    float limMax; // Salida máxima (ej. 100% de potencia)

    // --- Limitaciones del Integrador (Anti-Saturación) ---
    // Evita el crecimiento indefinido del término integral (Windup)
    // cuando el actuador ya está saturado.
    float limMinInt;
    float limMaxInt;

    // --- Tiempo de Muestreo ---
    float T;   // Periodo de ejecución del algoritmo en segundos

    // --- Variables de Estado (Memoria del sistema) ---
    float integrator; // Acumulador de la integral
    float prevError;  // Error de la iteración anterior (para derivada)
    float out;        // Última salida de control calculada

} PID_Controller;

// --- PROTOTIPOS ---
// Inicialización de la estructura y limpieza de variables de estado
void PID_Init(PID_Controller *pid, float kp, float ki, float kd, float t_sample, float min, float max);

// Ejecución de un paso del algoritmo de control
float PID_Compute(PID_Controller *pid, float referencia, float medicion);

#endif // PID_H