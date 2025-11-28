#include "max7219.h"

// --- CONTROL DE HARDWARE (Módulo SSI2 - Puerto D) ---
// Configuración a bajo nivel de los registros del microcontrolador

void SSI2_Init_LowLevel(void) {
    volatile uint32_t espera;
    
    // 1. Habilitar la señal de reloj para los periféricos
    SYSCTL_RCGCSSI_R  |= 0x04;  // Activar Módulo SSI2 (Bit 2)
    SYSCTL_RCGCGPIO_R |= 0x08;  // Activar Puerto D (Bit 3)
    espera = SYSCTL_RCGCGPIO_R; // Ciclo de espera para estabilización
    
    while((SYSCTL_PRGPIO_R & 0x08) == 0); // Verificar periférico listo
    
    // 2. Configuración de pines (Multiplexación)
    // PD1 = TX (Datos), PD3 = CLK (Reloj) -> Modo Función Alternativa
    // PD2 = CS (Selección de Chip) -> Modo GPIO Manual para control preciso
    
    GPIO_PORTD_AFSEL_R |= 0x0A;  // Activar función alterna en bits 1 y 3
    GPIO_PORTD_AFSEL_R &= ~0x04; // Desactivar función alterna en bit 2 (CS)
    
    // Configurar control de puerto (PCTL) para asignar SSI a PD1 y PD3
    // El valor 15 (0xF) corresponde a SSI en la matriz de pines del TM4C1294
    GPIO_PORTD_PCTL_R &= ~0x0000F0F0; // Limpiar configuración previa
    GPIO_PORTD_PCTL_R |=  0x0000F0F0; // Asignar función SSI
    
    // Habilitar funcionalidad digital en pines 1, 2 y 3
    GPIO_PORTD_DEN_R |= 0x0E; 
    
    // Configurar línea de Selección (CS) como Salida e iniciar en estado Alto (Inactivo)
    GPIO_PORTD_DIR_R  |= 0x04; 
    GPIO_PORTD_DATA_R |= 0x04; 

    // 3. Configuración del Módulo de Interfaz Serie Síncrona (SSI2)
    SSI2_CR1_R = 0; // Deshabilitar módulo durante la configuración
    
    // Configuración de velocidad (Prescaler)
    // Reloj Sistema (120 MHz) / 60 = 2 MHz (Velocidad de comunicación)
    SSI2_CPSR_R = 60; 
    
    // Configuración de trama: 
    // Fase/Polaridad (SPI Modo 0), Tamaño de datos: 8 bits (0x7)
    SSI2_CR0_R = 0x0007; 
    
    // Habilitar funcionamiento del SSI2
    SSI2_CR1_R |= 0x02; 
}

// Transmisión de comandos al controlador de pantalla
void MAX7219_Send(uint8_t registro, uint8_t dato) {
    // 1. Iniciar transacción: Bajar línea de Selección (CS - Activo Bajo)
    GPIO_PORTD_DATA_R &= ~0x04;
    
    // 2. Enviar dirección del registro
    // Verificar si hay espacio en la cola de transmisión (FIFO no llena)
    while((SSI2_SR_R & 0x02) == 0); 
    SSI2_DR_R = registro;
    
    // 3. Enviar dato de configuración o valor
    while((SSI2_SR_R & 0x02) == 0);
    SSI2_DR_R = dato;
    
    // 4. Esperar a que el módulo termine la transmisión (Bit de Ocupado)
    while(SSI2_SR_R & 0x10);
    
    // 5. Finalizar transacción: Subir línea de Selección (Latch de datos)
    GPIO_PORTD_DATA_R |= 0x04;
}

// Rutina de inicialización del driver MAX7219
void MAX7219_Init(void) {
    SSI2_Init_LowLevel();
    
    // Secuencia de configuración según hoja de datos
    MAX7219_Send(MAX7219_REG_SHUTDOWN, 0x01);    // Salir de modo suspensión (Wake up)
    MAX7219_Send(MAX7219_REG_DISPLAYTEST, 0x00); // Modo normal (No prueba de lámparas)
    MAX7219_Send(MAX7219_REG_DECODEMODE, 0xFF);  // Decodificación Código B (Dígitos 0-9)
    MAX7219_Send(MAX7219_REG_SCANLIMIT, 0x07);   // Escanear los 8 dígitos disponibles
    MAX7219_Send(MAX7219_REG_INTENSITY, 0x05);   // Intensidad luminosa media (Ciclo útil interno)
    
    MAX7219_Clear(); // Limpiar pantalla al inicio
}

// Apagar todos los segmentos
void MAX7219_Clear(void) {
    for(int i=1; i<=8; i++) {
        MAX7219_Send(i, 0x0F); // 0x0F es el código para "Espacio en blanco"
    }
}

// Función de visualización dividida para telemetría local
// Muestra el esfuerzo de control (PWM) a la izquierda y la variable de proceso (Encoder) a la derecha
void MAX7219_ShowStatus(uint32_t valor_pwm, uint32_t valor_encoder) {
    
    // --- Lado Derecho (Dígitos 0-3): Posición del Encoder ---
    // Recorte lógico para visualización de 4 dígitos (0-9999)
    if(valor_encoder > 9999) valor_encoder %= 10000; 
    
    MAX7219_Send(MAX7219_REG_DIGIT0, valor_encoder % 10);          // Unidades
    MAX7219_Send(MAX7219_REG_DIGIT1, (valor_encoder / 10) % 10);   // Decenas
    MAX7219_Send(MAX7219_REG_DIGIT2, (valor_encoder / 100) % 10);  // Centenas
    MAX7219_Send(MAX7219_REG_DIGIT3, (valor_encoder / 1000) % 10); // Millares

    // --- Lado Izquierdo (Dígitos 4-7): Porcentaje de PWM ---
    // Formato visual: "P XXX" o "- XXX"
    
    // Dígitos numéricos del valor PWM
    MAX7219_Send(MAX7219_REG_DIGIT4, valor_pwm % 10); // Unidades
    
    // Supresión de ceros a la izquierda para decenas
    if(valor_pwm >= 10)
        MAX7219_Send(MAX7219_REG_DIGIT5, (valor_pwm / 10) % 10);
    else
        MAX7219_Send(MAX7219_REG_DIGIT5, 0x0F); // Apagar dígito
        
    // Supresión de ceros a la izquierda para centenas (caso 100%)
    if(valor_pwm >= 100)
        MAX7219_Send(MAX7219_REG_DIGIT6, 1);
    else
        MAX7219_Send(MAX7219_REG_DIGIT6, 0x0F); // Apagar dígito

    // Dígito 7: Indicador de parámetro (Guión '-')
    // Código 0x0A corresponde al guión en modo decodificación B
    MAX7219_Send(MAX7219_REG_DIGIT7, 0x0A); 
}