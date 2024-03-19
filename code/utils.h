#ifndef _UTILS_H
#define _UTILS_H

void utils_setup();
void utils_delay(uint32_t ms);

uint8_t utils_get_bit(uint8_t * data, uint16_t position);
uint16_t utils_get_bits(uint8_t * data, uint16_t position, uint8_t len);

#endif // _UTILS_H