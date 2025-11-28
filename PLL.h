#ifndef PLL_H_
#define PLL_H_

#include <stdint.h>

/* Configuración de la frecuencia del sistema.
   Cambia el valor de PSYSDIV para ajustar la velocidad.
   
   PSYSDIV  SysClk (Hz)
     3     120,000,000
     4      96,000,000
     5      80,000,000
     7      60,000,000
     9      48,000,000
    15      30,000,000
    19      24,000,000
    29      16,000,000
    39      12,000,000
    79       6,000,000
*/
#define PSYSDIV 3 //120 MHZ

void PLL_Init(void);

#endif /* PLL_H_ */