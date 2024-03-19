#ifndef _SETTINGS_H
#define _SETTINGS_H

// ----------------------------------------------------------------------------
// General
// ----------------------------------------------------------------------------

#define APP_MANUFACTURER                "XOSE"
#define APP_NAME                        "CAT LITTER MONITOR"
#define APP_VERSION                     "v1.0.0"

// ----------------------------------------------------------------------------
// Debug
// ----------------------------------------------------------------------------

#ifndef DEBUG_LEVEL
#define DEBUG_LEVEL                     2
#endif
  
// ---------------------------------------------------
// LORAWAN
// ---------------------------------------------------

#define LORAWAN_DATARATE                DR_5		  		/* LoRaMac datarates definition, from DR_0 (slower) to DR_5 (faster) */
#define LORAWAN_ADR		                LORAWAN_ADR_OFF
#define LORAWAN_TX_POWER                TX_POWER_7	  		/* LoRaMac tx power definition, from TX_POWER_0 (Max EIRP) to TX_POWER_7 (Max EIRP - 14) */
#define JOINREQ_NBTRIALS                5			  		/* < Number of trials for the join request. */
#define LORAWAN_CLASS                   CLASS_A
#define LORAWAN_PORT                    1
#define LORAWAN_CONFIRM                 LMH_UNCONFIRMED_MSG

#include "credentials.h"

// ----------------------------------------------------------------------------
// Sending
// ----------------------------------------------------------------------------

#ifdef PRODUCTION
#define READ_INTERVAL_SECONDS           (60*5)
#else
#define READ_INTERVAL_SECONDS           (60*1)
#endif

// ----------------------------------------------------------------------------
// Watchdog timer
// ----------------------------------------------------------------------------

#define WDT_SECONDS                     (READ_INTERVAL_SECONDS+10)

// ----------------------------------------------------------------------------
// States et al.
// ----------------------------------------------------------------------------

enum {
    STATE_SLEEPING,
    STATE_RECORDING
};

// ----------------------------------------------------------------------------
// Sleep interrupt
// ----------------------------------------------------------------------------

#define INTERRUPT_GPIO                  WB_IO6
#define INTERRUPT_TRIGGER               CHANGE

// ----------------------------------------------------------------------------
// Battery
// ----------------------------------------------------------------------------

#define BAT_MEASUREMENT_GPIO            WB_A0

#define VBAT_MV_PER_LSB                 (0.73242188F)   // 3.0V ADC range and 12-bit ADC resolution = 3000mV/4096
#define VBAT_DIVIDER                    (0.4F)          // 1.5M + 1M voltage divider on VBAT = (1.5M / (1M + 1.5M))
#define VBAT_DIVIDER_COMP               (1.709F)        // Compensation factor for the VBAT divider
#define REAL_VBAT_MV_PER_LSB            (VBAT_DIVIDER_COMP * VBAT_MV_PER_LSB)

#define BATT_MIN_MV                     3300
#define BATT_MAX_MV                     4100

// ---------------------------------------------------
// Sensors
// ---------------------------------------------------

#define SENSOR_POWER_GPIO               WB_IO2

#endif // _SETTINGS_H

