#ifndef _WDT_H
#define _WDT_H

void wdt_setup(unsigned long seconds);
void wdt_disable();
void wdt_feed();
bool wdt_triggered();

#endif