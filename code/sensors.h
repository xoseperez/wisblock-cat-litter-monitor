#ifndef _H_SENSORS
#define _H_SENSORS

void sensors_power(bool power);
uint16_t sensors_voc_raw();
int32_t sensors_voc_index();
float sensors_temperature();
uint8_t sensors_humidity();
void sensors_read();
bool sensors_setup();

#endif // _H_SENSORS