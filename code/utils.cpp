#include <Arduino.h>

#include "utils.h"
#include "log.h"
#include "settings.h"

char _utils_device_name[32] = {0};

void utils_delay(uint32_t ms) {
    uint32_t start = millis();
    while (millis() - start < ms) delay(1);
}

uint8_t utils_get_bit(uint8_t * data, uint16_t position) {
    uint8_t byte = position >> 3;
    uint8_t bit = 7 - (position & 0x07);
    uint8_t value = (data[byte] >> bit) & 0x01;
    return value;
}

uint16_t utils_get_bits(uint8_t * data, uint16_t position, uint8_t len) {
    uint16_t value = 0;
    for (uint8_t i=0; i<len; i++) {
        uint8_t bit = position + i;
        value = (value << 1) + utils_get_bit(data, bit);
    }
    return value;
}

