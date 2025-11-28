#include <stdint.h>
#include "PLL.h"
#include "tm4c1294ncpdt.h"

//La frecuencia de PLL fVCO=480 Mhz

/* Funciona que nos permite manipular la frecuencia de trabajo de la tiva C activando el PLL
y definiendo los par?metros para un conjunto de frecuencias definidas en la tabla*/
#define FXTAL 25000000
#define Q 0
#define N 4
#define MINT 96
#define MFRAC 0

// Cálculo para los Wait States
#define SYSCLK_CALC ((FXTAL/((Q+1)*(N+1))) * (MINT + (MFRAC/1024))) / (PSYSDIV+1)

void PLL_Init(void) {
    uint32_t timeout;

    // 1. Configurar Osciladores
    SYSCTL_RSCLKCFG_R &= ~SYSCTL_RSCLKCFG_USEPLL; // Apagar uso de PLL temporalmente
    
    // Encender MOSC
    SYSCTL_MOSCCTL_R &= ~(SYSCTL_MOSCCTL_NOXTAL | SYSCTL_MOSCCTL_PWRDN);
    
    // Esperar a que el MOSC esté listo
    while((SYSCTL_RIS_R & SYSCTL_RIS_MOSCPUPRIS) == 0){}; 
        
    SYSCTL_RSCLKCFG_R = (SYSCTL_RSCLKCFG_R & ~(0x0F000000 | SYSCTL_RSCLKCFG_OSCSRC_M)) | 
                        (0x03000000) |             // PLLSRC = MOSC
                        SYSCTL_RSCLKCFG_OSCSRC_MOSC;
    
    // Configurar reloj para Deep Sleep
    SYSCTL_DSCLKCFG_R = (SYSCTL_DSCLKCFG_R & ~SYSCTL_DSCLKCFG_DSOSCSRC_M) | SYSCTL_DSCLKCFG_DSOSCSRC_MOSC;                                                                    

    // 2. Configurar PLL (fVCO = 480 MHz)
    SYSCTL_PLLFREQ0_R = (SYSCTL_PLLFREQ0_R & ~SYSCTL_PLLFREQ0_MFRAC_M) + (MFRAC << SYSCTL_PLLFREQ0_MFRAC_S) |
                        (SYSCTL_PLLFREQ0_R & ~SYSCTL_PLLFREQ0_MINT_M) + (MINT << SYSCTL_PLLFREQ0_MINT_S);

    SYSCTL_PLLFREQ1_R = (SYSCTL_PLLFREQ1_R & ~SYSCTL_PLLFREQ1_Q_M) + (Q << SYSCTL_PLLFREQ1_Q_S) |
                        (SYSCTL_PLLFREQ1_R & ~SYSCTL_PLLFREQ1_N_M) + (N << SYSCTL_PLLFREQ1_N_S);    
    
    SYSCTL_PLLFREQ0_R |= SYSCTL_PLLFREQ0_PLLPWR; // Prender PLL
    SYSCTL_RSCLKCFG_R |= SYSCTL_RSCLKCFG_NEWFREQ; // Trigger nueva frecuencia

    // 3. Configurar Flash Wait States (MEMTIM0)
    // El cálculo se basa en la frecuencia objetivo (120 MHz con PSYSDIV 3)
    if (SYSCLK_CALC < 16000000) {
        SYSCTL_MEMTIM0_R = (SYSCTL_MEMTIM0_R & ~0x03EF03EF) | (0x0<<22) | (0x0<<21) | (0x0<<16) | (0x0<<6) | (0x0<<5) | (0x0);
    } else if(SYSCLK_CALC == 16000000) {
        SYSCTL_MEMTIM0_R = (SYSCTL_MEMTIM0_R & ~0x03EF03EF) | (0x0<<22) | (0x1<<21) | (0x0<<16) | (0x0<<6) | (0x1<<5) | (0x0);
    } else if (SYSCLK_CALC <= 40000000) {
        SYSCTL_MEMTIM0_R = (SYSCTL_MEMTIM0_R & ~0x03EF03EF) | (0x2<<22) | (0x0<<21) | (0x1<<16) | (0x2<<6) | (0x0<<5) | (0x1);
    } else if (SYSCLK_CALC <= 60000000) {
        SYSCTL_MEMTIM0_R = (SYSCTL_MEMTIM0_R & ~0x03EF03EF) | (0x3<<22) | (0x0<<21) | (0x2<<16) | (0x3<<6) | (0x0<<5) | (0x2);
    } else if (SYSCLK_CALC <= 80000000) {
        SYSCTL_MEMTIM0_R = (SYSCTL_MEMTIM0_R & ~0x03EF03EF) | (0x4<<22) | (0x0<<21) | (0x3<<16) | (0x4<<6) | (0x0<<5) | (0x3);    
    } else if (SYSCLK_CALC <= 100000000) {
        SYSCTL_MEMTIM0_R = (SYSCTL_MEMTIM0_R & ~0x03EF03EF) | (0x5<<22) | (0x0<<21) | (0x4<<16) | (0x5<<6) | (0x0<<5) | (0x4);
    } else if (SYSCLK_CALC <= 120000000) {
        SYSCTL_MEMTIM0_R = (SYSCTL_MEMTIM0_R & ~0x03EF03EF) | (0x6<<22) | (0x0<<21) | (0x5<<16) | (0x6<<6) | (0x0<<5) | (0x5);
    } else {                            
        return;
    }
    
    // 4. Esperar Lock
    timeout = 0;
    while(((SYSCTL_PLLSTAT_R & SYSCTL_PLLSTAT_LOCK) == 0) && (timeout < 0xFFFF)){
        timeout++;
    }
    if(timeout == 0xFFFF) return;
    
    // 5. Activar SysClk
    SYSCTL_RSCLKCFG_R = (SYSCTL_RSCLKCFG_R & ~SYSCTL_RSCLKCFG_PSYSDIV_M) | 
                        (PSYSDIV & SYSCTL_RSCLKCFG_PSYSDIV_M) |
                        SYSCTL_RSCLKCFG_MEMTIMU | 
                        SYSCTL_RSCLKCFG_USEPLL;
}