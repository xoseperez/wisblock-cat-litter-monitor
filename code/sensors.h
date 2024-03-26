#ifndef _H_SENSORS
#define _H_SENSORS

#include "settings.h"

void sensors_power(bool power);
void sensors_sleep(bool sleep);

#if SENSOR_MQ135_ENABLE
uint16_t sensors_nh3();
#endif

#if SENSOR_SGP40_ENABLE
uint16_t sensors_voc_raw();
int32_t sensors_voc_index();
#endif

#if SENSOR_SHTC3_ENABLE
float sensors_temperature();
uint8_t sensors_humidity();
#endif

void sensors_read();
bool sensors_setup();

#endif // _H_SENSORS