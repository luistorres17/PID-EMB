# Controlador PID para Señal PWM - STM32 🎛️

> **Estado:** 🚀 Activo

Este proyecto implementa un controlador **PID (Proporcional-Integral-Derivativo)** en lenguaje C para microcontroladores de la familia **STM32**. Su objetivo principal es mantener una señal PWM completamente estable, compensando automáticamente las perturbaciones del sistema en tiempo real para aplicaciones embebidas.

## 📋 Tabla de Contenidos
- [Descripción General](#descripción-general)
- [Arquitectura y Módulos](#arquitectura-y-módulos)
- [Requisitos de Hardware](#requisitos-de-hardware)
- [Entorno de Desarrollo](#entorno-de-desarrollo)
- [Instalación y Uso](#instalación-y-uso)

## 📖 Descripción General
El sistema utiliza un lazo de control cerrado para ajustar dinámicamente el ciclo de trabajo (*duty cycle*) de una señal PWM. Es una solución ideal para el control de motores, regulación de voltaje en convertidores de potencia o sistemas térmicos, asegurando que la variable del proceso converja rápidamente hacia el valor de referencia (*SetPoint*) con el mínimo error en estado estacionario.

## 🏗️ Arquitectura y Módulos
El firmware está estructurado de manera modular para garantizar un control a bajo nivel eficiente y escalable:

- **`PID.c` & `PID.h`**: Algoritmo central del controlador (cálculo de la acción de control mediante las ganancias Proporcional, Integral y Derivativa).
- **`max7219.c` & `max7219.h`**: Controlador para el driver de displays LED MAX7219. Permite la visualización en tiempo real de las variables del sistema (SetPoint, valor actual, etc.).
- **`PLL.c` & `PLL.h`**: Configuración de los relojes del sistema, fundamental para la precisión temporal del PID y la generación de la señal PWM.
- **`UART.c` & `UART.h`**: Interfaz de comunicación serial para la telemetría, monitoreo de las respuestas transitorias y posible ajuste paramétrico desde una consola.
- **`config.c` & `config.h`**: Abstracción para la inicialización y configuración de los periféricos de hardware del STM32 (Timers para PWM, ADC para retroalimentación, puertos GPIO).
- **`main.c`**: Bucle principal y orquestación del lazo de retroalimentación.

## 🔌 Requisitos de Hardware
- Placa de desarrollo basada en **STM32** (ej. placa Nucleo, Discovery o Blue Pill, dependiendo de la variante específica utilizada).
- Módulo de display/matriz LED basado en el circuito **MAX7219**.
- Sistema o planta a controlar (ej. puente H con motor DC, etapa de potencia, etc.).
- Programador ST-Link o cable USB compatible para programación y depuración.

## 🛠️ Entorno de Desarrollo
- **IDE:** Keil µVision (MDK-ARM).
- El repositorio incluye el proyecto estructurado para su importación directa mediante el archivo de proyecto `.uvprojx`.

## 🚀 Instalación y Uso
1. Clona o descarga este repositorio en tu entorno local.
2. Abre el archivo de proyecto `.uvprojx` en Keil µVision.
3. Asegúrate de tener instalados los *Device Family Packs* correspondientes a la familia STM32 que estés utilizando.
4. Compila el código fuente (*Project -> Build Target* o presionando `F7`).
5. Conecta tu placa STM32 mediante tu programador y carga el firmware en la memoria flash (*Download* o `F8`).
6. Conecta el hardware periférico de acuerdo a la distribución de pines establecida en el código para el PWM, el MAX7219 y la entrada analógica/digital.
7. (Opcional) Abre una terminal serial configurada al puerto COM respectivo para monitorear la telemetría del lazo de control.

---
*Desarrollado para proveer control robusto y determinista en aplicaciones de electrónica de potencia y sistemas embebidos.*
