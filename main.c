/* main.c - Sistema de Control de Posición - TUNED para Plotter */
#include "config.h"
#include "max7219.h"
#include "PID.h"
#include "PLL.h"
#include "UART.h"

// Instancia del controlador
PID_Controller systemPID;

// Configuración de Tiempos
#define SAMPLE_TIME_MS  25      //40 hz 25 ms
#define SAMPLE_TIME_SEC 0.05f

// Parámetros de Operación
#define TARGET_POS      435.0f  // Objetivo (Cuentas del encoder)
#define PWM_MIN         14.0f   // valor minimo de pwm con el que reaciona el motor a 2khz
#define PWM_MAX         99.0f   // maximo valor de pwm con el que reaciona el motor 

// Valor de PWM necesario para mantener el helicóptero flotando estable
#define PWM_EQUILIBRIO  45.0f   

int main(void) {
	// 1. Configuración inicial del Hardware
	Config_FPU();      // activacion del fpu     
	PLL_Init();  
    
	Config_PWM();           
	Config_QEI();           
	Config_UART0_115200();  
	Config_SSI2_Display();  
    
	Delay_ms(100);          
	MAX7219_Init();         
	__asm("cpsie i");       

    
	float pid_correction_max = 54.0f;
	float pid_correction_min = -31.0f;


	PID_Init(&systemPID, 0.9f, 0.4f, 1.0f, SAMPLE_TIME_SEC, pid_correction_min, pid_correction_max);
    
    
    

	// Bucle principal
	while(1) {
		// A. Leer sensor
		uint32_t current_pos = QEI_GetPosition();
    // B. Calcular corrección (Algoritmo PID)
		float pid_out = PID_Compute(&systemPID, TARGET_POS, (float)current_pos);
    float pwm_total = PWM_EQUILIBRIO + pid_out;
		// Protección final de rangos 
		if (pwm_total > PWM_MAX) pwm_total = PWM_MAX;
		if (pwm_total < PWM_MIN) pwm_total = PWM_MIN;
		uint32_t pwm_duty = (uint32_t)pwm_total;
		//Actuar sobre el sistema físico
		PWM_SetDuty(pwm_duty);
		//Mostrar información en display
		MAX7219_ShowStatus(pwm_duty, current_pos);
		// ---------------------------------------------------------
		// 1. Calcular el error en tiempo real (Float a Entero)
		// Error = Referencia - Lectura Actual.
		// Si Error > 0: Estamos debajo del objetivo.
		// Si Error < 0: Nos pasamos del objetivo.
		float error_val = TARGET_POS - (float)current_pos;
		int32_t error_to_print = (int32_t)error_val;

		// 2. Enviar por UART manejando el signo manualmente
		// UART0_PrintInt solo imprime unsigned, así que si es negativo
		// imprimimos el '-' y luego el valor absoluto.
		if (error_to_print < 0) {
			UART0_SendChar('-');              
			UART0_PrintInt((uint32_t)(-error_to_print)); 
    } 
		else{
			UART0_PrintInt((uint32_t)error_to_print);
    }
		// 3. Final de línea para que el plotter detecte el dato
		UART0_PrintString("\r\n");
        // ---------------------------------------------------------
    Delay_ms(SAMPLE_TIME_MS);
    }
}