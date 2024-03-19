#ifndef _BATTERY_H
#define _BATTERY_H

unsigned long battery_read();
unsigned char battery_read_percent();
void battery_setup();

#endif // _BATTERY_H