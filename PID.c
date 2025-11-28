/* pid.c - Implementación del Algoritmo de Control */
#include "PID.h"

// Inicialización de la estructura y parámetros del controlador
void PID_Init(PID_Controller *pid, float kp, float ki, float kd, float t_sample, float min, float max) {
    pid->Kp = kp;
    pid->Ki = ki;
    pid->Kd = kd;
    pid->T  = t_sample; // Tiempo de muestreo
    
    // Definir los límites permitidos para la salida (Actuador)
    pid->limMin = min;
    pid->limMax = max;
    
    // Límites para la parte integral (anti-saturación)
    pid->limMinInt = min;
    pid->limMaxInt = max;

    // Limpiar memoria histórica
    pid->integrator = 0.0f;
    pid->prevError  = 0.0f;
    pid->out        = 0.0f;
}

// Función principal de cálculo del PID
float PID_Compute(PID_Controller *pid, float setpoint, float measurement) {
    // 1. Calcular el Error (Diferencia entre lo deseado y lo real)
    float error = setpoint - measurement;

    // 2. Parte Proporcional: Reacción inmediata al error
    float proportional = pid->Kp * error;

    // 3. Parte Integral: Acumulación del error en el tiempo (corrige error estacionario)
    // Usamos aproximación trapezoidal
    pid->integrator += 0.5f * pid->Ki * pid->T * (error + pid->prevError);

    // Protección anti-saturación (Anti-windup):
    // Si la suma integral crece demasiado, la recortamos para que no afecte la recuperación
    if (pid->integrator > pid->limMaxInt) {
        pid->integrator = pid->limMaxInt;
    } else if (pid->integrator < pid->limMinInt) {
        pid->integrator = pid->limMinInt;
    }

    // 4. Parte Derivativa: Predicción basada en la velocidad de cambio del error
    float derivative = pid->Kd * (error - pid->prevError) / pid->T;

    // 5. Suma total de las tres acciones
    pid->out = proportional + pid->integrator + derivative;

    // 6. Limitación final de salida (Clamping)
    // Aseguramos que el valor enviado al motor esté dentro de su rango operativo real
    if (pid->out > pid->limMax) {
        pid->out = pid->limMax;
    } else if (pid->out < pid->limMin) {
        pid->out = pid->limMin;
    }

    // 7. Guardar el error actual para el siguiente cálculo
    pid->prevError = error;

    return pid->out;
}