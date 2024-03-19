#include <Arduino.h>

#include "battery.h"
#include "settings.h"
#include "log.h"

// ----------------------------------------------------------------------------
// Public methods
// ----------------------------------------------------------------------------

unsigned long battery_read() {
    float raw = analogRead(BAT_MEASUREMENT_GPIO);
    unsigned long mv = raw * REAL_VBAT_MV_PER_LSB;
    logWrite(LOG_DEBUG, "BATT", "Battery reading: %d", mv);
    return mv;
    
}

unsigned char battery_read_percent() {
    unsigned long mv = battery_read();
    mv = constrain(mv, BATT_MIN_MV, BATT_MAX_MV);
    return map(mv, BATT_MIN_MV, BATT_MAX_MV, 0, 100);
}

void battery_setup() {
    pinMode(BAT_MEASUREMENT_GPIO, INPUT);
    analogReference(AR_INTERNAL_3_0);
    analogReadResolution(12);
}
