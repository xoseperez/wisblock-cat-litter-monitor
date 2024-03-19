#ifndef _LORAWAN_H
#define _LORAWAN_H

#include <LoRaWan-RAK4630.h>
#include "settings.h"

#define PIN_LORA_DIO_1 47

typedef enum {
    LORAWAN_TYPE_JOINED,
    LORAWAN_TYPE_JOIN_FAILED,
    LORAWAN_TYPE_UPLINK_CONF,
    LORAWAN_TYPE_UPLINK_UNCONF,
    LORAWAN_TYPE_DOWNLINK,
} lorawan_message_type;

bool lorawan_send(unsigned char * data, unsigned char len, unsigned char port = LORAWAN_PORT, lmh_confirm type = LORAWAN_CONFIRM);
bool lorawan_setup(void (*)(lorawan_message_type type, lmh_app_data_t * data));

#endif // _LORAWAN_H