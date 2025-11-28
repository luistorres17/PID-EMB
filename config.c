/* config.c */
#include "config.h"

// Constantes para el PWM
// Valor calculado para obtener una frecuencia de 2kHz con el reloj a 120MHz
#define PWM_PERIOD  60000 

// -----------------------------------------------------------------------------
// CONFIGURACIÓN DEL SISTEMA
// -----------------------------------------------------------------------------
void Config_FPU(void) {
    // Habilitar acceso completo al coprocesador (CP10 y CP11)
    NVIC_CPAC_R |= 0x00F00000; 
    __asm(" nop"); __asm(" nop"); __asm(" nop"); // Espera breve para asegurar activación
}

// Función de retardo simple (bloqueante)
// No es precisa, solo para esperas aproximadas
void Delay_ms(uint32_t ms) {
    uint32_t i, j;
    for(i = 0; i < ms; i++) {
        // Ciclo calibrado experimentalmente para ~1ms a 120MHz
        for(j = 0; j < 24000; j++) { __asm(" nop"); }
    }
}

// -----------------------------------------------------------------------------
// CONFIGURACIÓN DEL ACTUADOR PWM (Pin PF0)
// -----------------------------------------------------------------------------
void Config_PWM(void) {
    // Habilitar reloj para el módulo PWM0 y el puerto F
    SYSCTL_RCGCPWM_R  |= 0x01; 
    SYSCTL_RCGCGPIO_R |= 0x20; 
    
    // Esperar a que el periférico esté listo
    while((SYSCTL_PRGPIO_R & 0x20) == 0){};
    
    // Configurar PF0 como salida de función alternativa
    GPIO_PORTF_AFSEL_R |= 0x01;  
    GPIO_PORTF_PCTL_R  = (GPIO_PORTF_PCTL_R & 0xFFFFFFF0) | 0x00000006; // Mux 6 = M0PWM0
    GPIO_PORTF_DEN_R   |= 0x01; // Habilitar digital        

    // --- CORRECCIÓN: Desactivar Drenador Abierto (Push-Pull) ---
    // Limpiamos el bit 0 de ODR para asegurar que sea Push-Pull.
    // Ahora el microcontrolador "empujará" 3.3V cuando sea 1 lógico.
    GPIO_PORTF_ODR_R   &= ~0x01; 
    // -----------------------------------------------------------

    // Configuración del Generador 0
    PWM0_0_CTL_R = 0; // Deshabilitar para configurar
    PWM0_CC_R = 0;    // Usar reloj del sistema
    
    // Asumiendo que PWM_PERIOD está definido globalmente en config.c (ej. 60000)
    PWM0_0_LOAD_R = PWM_PERIOD - 1; 
    
    PWM0_0_CMPA_R = 0; // Iniciar apagado (0% potencia)
    PWM0_0_GENA_R = 0x000000C8; // Acción: Bajar en coincidencia A, Subir en carga
    
    // Habilitar generador y salida
    PWM0_0_CTL_R |= 1; 
    PWM0_ENABLE_R |= 1; 
}

// Función para ajustar la potencia del motor
// duty: Valor entre 0 y 100
void PWM_SetDuty(uint32_t duty) {
    // Protección por software: limitar a 100%
    if (duty > 100) duty = 100;
    
    // Convertir porcentaje a cuentas del temporizador
    uint32_t new_cmp = (PWM_PERIOD * duty) / 100;
    
    // Límites de seguridad para evitar errores del hardware
    if (new_cmp < 1) new_cmp = 1;
    if (new_cmp > PWM_PERIOD - 1) new_cmp = PWM_PERIOD - 1;

    // Sección crítica: deshabilitar interrupciones mientras escribimos
    __asm("cpsid i"); 
    PWM0_0_CMPA_R = new_cmp;
    __asm("cpsie i"); 
}

// -----------------------------------------------------------------------------
// SENSOR ENCODER (QEI) - Puerto L
// -----------------------------------------------------------------------------
void Config_QEI(void) {
    // Habilitar reloj para QEI0 y puerto L
    SYSCTL_RCGCQEI_R |= 0x01;  
    SYSCTL_RCGCGPIO_R |= 0x400; 
    while((SYSCTL_PRGPIO_R & 0x400) == 0){}; 

    // Configurar pines PL1, PL2 (PhA, PhB) y PL3 (Index)
    GPIO_PORTL_AFSEL_R |= 0x0E; 
    GPIO_PORTL_PCTL_R = (GPIO_PORTL_PCTL_R & 0xFFFF000F) | 0x00006660; // Función alternativa QEI
    GPIO_PORTL_DEN_R |= 0x0E;
    GPIO_PORTL_PUR_R |= 0x0E; // Resistencias pull-up internas
    
    while((SYSCTL_PRQEI_R & 0x01) == 0){};

    // Configuración del módulo QEI
    QEI0_CTL_R &= ~0x00000001; // Deshabilitar
    QEI0_CTL_R &= ~0x000F0000; // Limpiar configuración previa
    QEI0_CTL_R |= 0x00000018;  // Captura en ambos bordes (Modo x4)

    // Límite máximo del contador
    // Se pone 939 para dar un pequeño margen sobre el recorrido físico real (938)
    // y evitar reinicios bruscos a cero si hay rebote mecánico al final.
    QEI0_MAXPOS_R = 1000; 
    
    QEI0_POS_R = 0; // Reiniciar posición actual
    QEI0_CTL_R |= 0x00000001; // Habilitar
}

// Leer la posición actual del encoder
uint32_t QEI_GetPosition(void) {
    return QEI0_POS_R;
}

// -----------------------------------------------------------------------------
// COMUNICACIÓN (PANTALLA Y PUERTO SERIE)
// -----------------------------------------------------------------------------
void Config_SSI2_Display(void) {
    // Configuración del protocolo SPI para el display
    SYSCTL_RCGCSSI_R  |= 0x04;  
    SYSCTL_RCGCGPIO_R |= 0x08;  
    while((SYSCTL_PRGPIO_R & 0x08) == 0){}; 
    
    // Configurar pines del Puerto D
    GPIO_PORTD_AFSEL_R |= 0x0A; // Pines alternativos
    GPIO_PORTD_PCTL_R = (GPIO_PORTD_PCTL_R & 0xFFFF0000) | 0x0000F0F0;
    GPIO_PORTD_DEN_R   |= 0x0A;
    
    // Chip Select manual (PD2)
    GPIO_PORTD_AFSEL_R &= ~0x04; 
    GPIO_PORTD_DIR_R   |= 0x04;  
    GPIO_PORTD_DEN_R   |= 0x04;  
    GPIO_PORTD_DATA_R  |= 0x04;  // Iniciar en alto (deseleccionado)
    
    // Configuración SPI: Maestro, 1MHz aprox
    SSI2_CR1_R = 0;        
    SSI2_CPSR_R = 60; // Divisor de reloj
    SSI2_CR0_R = 0x0007; // 8 bits de datos
    SSI2_CR1_R |= 0x02; // Habilitar SSI
}

void SPI_WriteByte(uint8_t data) {
    // Esperar si la cola de transmisión está llena
    while((SSI2_SR_R & 0x00000002) == 0){}; 
    SSI2_DR_R = data; // Escribir dato
    // Esperar a que termine de transmitir (Bit Busy)
    while(SSI2_SR_R & 0x00000010){}; 
}

// Funciones para controlar el pin de selección (Chip Select)
void CS_Select(void) { GPIO_PORTD_DATA_R &= ~0x04; } // Bajar pin
void CS_Deselect(void) { GPIO_PORTD_DATA_R |= 0x04; } // Subir pin