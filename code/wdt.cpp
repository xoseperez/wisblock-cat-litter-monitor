#include <Arduino.h>
#include "wdt.h"

// https://github.com/NordicSemiconductor/nrfx/blob/master/mdk/nrf52820_bitfields.h

#if defined(__CC_ARM) || (defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050))
__attribute__ ((section(".bss.uninitialised"),zero_init)) unsigned int _wdt_retained;
#elif defined(__GNUC__)
__attribute__ ((section(".uninitialised"))) unsigned int _wdt_retained;
#elif defined(__ICCARM__)
unsigned int _wdt_retained @ ".uninitialised";
#endif

// The WDT interrupt handler will have around 2 32kHz clock cycles to execute before reset.
void WDT_IRQHandler(void) {
    _wdt_retained = 1;
    NRF_WDT->EVENTS_TIMEOUT = 0;    
}

bool wdt_triggered() {
    bool output = (_wdt_retained == 1);
    _wdt_retained = 0;
    return output;
}

void wdt_setup(unsigned long seconds) {
    _wdt_retained = 0;
    NRF_WDT->CONFIG         = (WDT_CONFIG_HALT_Pause << WDT_CONFIG_HALT_Pos) | ( WDT_CONFIG_SLEEP_Run << WDT_CONFIG_SLEEP_Pos);
    NRF_WDT->CRV            = 32768 * seconds + 1;
    NRF_WDT->RREN          |= WDT_RREN_RR0_Msk;
    NVIC_SetPriority(WDT_IRQn, 7);
    NVIC_ClearPendingIRQ(WDT_IRQn);
    NVIC_EnableIRQ(WDT_IRQn);
    NRF_WDT->INTENSET       = WDT_INTENSET_TIMEOUT_Msk;	    
    NRF_WDT->TASKS_START    = 1;
}

void wdt_disable() {
    NRF_WDT->CONFIG         = (WDT_CONFIG_HALT_Pause << WDT_CONFIG_HALT_Pos) | ( WDT_CONFIG_SLEEP_Pause << WDT_CONFIG_SLEEP_Pos);
    NRF_WDT->CRV            = 4294967295;
}

void wdt_feed() {
    if ((bool)(NRF_WDT->RUNSTATUS) == true) {
        NRF_WDT->RR[0] = WDT_RR_RR_Reload;
    }
}

