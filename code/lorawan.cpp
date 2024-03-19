#include <Arduino.h>
#include "settings.h"
#include "lorawan.h"
#include "battery.h"
#include "log.h"

/** Semaphore used by events to wake up loop task */
SemaphoreHandle_t loraEvent = NULL;

/** LoRa task handle */
TaskHandle_t lorawan_task_handle;

void (*_lorawan_message)(lorawan_message_type type, lmh_app_data_t * data);
void lorawan_sleep();

// ----------------------------------------------------------------------------

static void lorawan_rx_handler(lmh_app_data_t * data) {

	logWrite(LOG_DEBUG, "LORA", 
		"LoRa Packet received on port %d, size:%d, rssi:%d, snr:%d, data:%s",
		data->port, data->buffsize, data->rssi, data->snr, data->buffer
	);

	if (_lorawan_message) {
		_lorawan_message(LORAWAN_TYPE_DOWNLINK, data);
	}

	// Stop radio processing
	lorawan_sleep();

}

static void lorawan_has_joined_handler(void) {

	//logWrite(LOG_DEBUG, "LORA", "OTAA Mode, Network Joined!");

	if (_lorawan_message) {
		_lorawan_message(LORAWAN_TYPE_JOINED, nullptr);
	}

	// Stop radio processing
	lorawan_sleep();

}

static void lorawan_join_failed_handler(void) {
  
	//logWrite(LOG_DEBUG, "LORA", "Join failed!");

	if (_lorawan_message) {
		_lorawan_message(LORAWAN_TYPE_JOIN_FAILED, nullptr);
	}

}

static void lorawan_unconf_finished(void) {

	//logWrite(LOG_DEBUG, "LORA", "Message sent!");

	if (_lorawan_message) {
		_lorawan_message(LORAWAN_TYPE_UPLINK_UNCONF, nullptr);
	}

	// Stop radio processing
	lorawan_sleep();

}

static void lorawan_conf_result(bool result) {

	//logWrite(LOG_DEBUG, "LORA", "Message sent!");

	if (_lorawan_message) {
		_lorawan_message(LORAWAN_TYPE_UPLINK_CONF, nullptr);
	}

	// Stop radio processing
	lorawan_sleep();

}

static void lorawan_confirm_class_handler(DeviceClass_t Class) {

	//logWrite(LOG_DEBUG, "LORA", "Switch to class %c done", "ABC"[Class]);

	// Inform the server that switch has occurred ASAP
	lmh_app_data_t m_lora_app_data = { 
		nullptr, 		// buffer
		0, 				// buffer size
		LORAWAN_PORT, 	// port
		0, 				// RSSI
		0				// SNR
	};
	lmh_send(&m_lora_app_data, LORAWAN_CONFIRM);

}

static uint8_t lorawan_get_battery_level(void) {
	return battery_read_percent();
}

static void lora_interrupt_handler(void) {

	// Wake up LoRa task
	if (loraEvent != NULL) {
		xSemaphoreGiveFromISR(loraEvent, pdFALSE);
	}

}

// ----------------------------------------------------------------------------

void lorawan_sleep() {

	// Set the radio module to sleep
	//Radio.Sleep();	

    // Send LoRa handler back to sleep
    xSemaphoreTake(loraEvent, 10);	

}

bool lorawan_send(unsigned char * data, unsigned char len, unsigned char port, lmh_confirm type) {

	if (lmh_join_status_get() != LMH_SET) {
		//Not joined, try again later
		return false;
	}

	logWrite(LOG_DEBUG, "LORA", "Sending frame (length: %d bytes)", len);
	for (unsigned char i=0; i<len; i++) {
		logWriteRaw(LOG_DEBUG, "%02X", data[i]);
	}
	logWriteRaw(LOG_DEBUG, "\n");

	// Build message structure
	lmh_app_data_t m_lora_app_data = { 
		data, 			// buffer
		len, 			// buffer size
		LORAWAN_PORT, 	// port
		0, 				// RSSI
		0				// SNR
	};
	lmh_error_status error = lmh_send(&m_lora_app_data, LORAWAN_CONFIRM);

	// Allow IRQ processing
	xSemaphoreGive(loraEvent);

	return (error == LMH_SUCCESS);

}

void lorawan_task(void *pvParameters) {

	while(true) {

        if (xSemaphoreTake(loraEvent, portMAX_DELAY) == pdTRUE) {
			
			// Handle Radio events
			Radio.IrqProcessAfterDeepSleep();

		}
	}

}

/**
 * @brief Structure containing LoRaWan callback functions, needed for lmh_init()
 */
static lmh_callback_t lora_callbacks = {
    lorawan_get_battery_level, BoardGetUniqueId, BoardGetRandomSeed,
	lorawan_rx_handler, lorawan_has_joined_handler, lorawan_confirm_class_handler, 
	lorawan_join_failed_handler, lorawan_unconf_finished, lorawan_conf_result
};

bool lorawan_setup(void (*lorawan_message)(lorawan_message_type type, lmh_app_data_t * data)) {
    
    // Create the LoRaWan event semaphore
	loraEvent = xSemaphoreCreateBinary();
	
	// Initialize semaphore
	xSemaphoreGive(loraEvent);

	// Set callback
	_lorawan_message = lorawan_message;

    // Initialize LoRa chip.
	lora_rak4630_init();

	// Init structure
	lmh_param_t lora_param_init = {
    	LORAWAN_ADR, LORAWAN_DATARATE, LORAWAN_PUBLIC_NETWORK, JOINREQ_NBTRIALS, LORAWAN_TX_POWER, LORAWAN_DUTYCYCLE_OFF
	};

	// Initialize LoRaWan
	unsigned long err_code = lmh_init(&lora_callbacks, lora_param_init, true, CLASS_A, LORAMAC_REGION_EU868);
	if (err_code != 0) {
		logWrite(LOG_INFO, "LORA", "LoRa init failed with error: %d", err_code);
		return false;
	}

	// In deep sleep we need to hijack the SX126x IRQ to trigger a wakeup of the nRF52
	attachInterrupt(PIN_LORA_DIO_1, lora_interrupt_handler, RISING);
	
	// Background task
	if (!xTaskCreate(lorawan_task, "LORA", 8192, NULL, TASK_PRIO_NORMAL, &lorawan_task_handle)) {
		logWrite(LOG_INFO, "LORA", "Failed to start lorawan task");
		return false;
	}

	// Setup the EUIs and Keys
	unsigned char deveui[] = LORAWAN_DEVEUI;
	unsigned char appeui[] = LORAWAN_APPEUI;
	unsigned char appkey[] = LORAWAN_APPKEY;
	lmh_setDevEui(deveui);
	lmh_setAppEui(appeui);
	lmh_setAppKey(appkey);
	
	// Debug
	logWrite(LOG_INFO, "LORA", 
		"Device EUI: %02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X", 
		deveui[0], deveui[1], deveui[2], deveui[3],
		deveui[4], deveui[5], deveui[6], deveui[7]
	);

	// Start Join procedure
	lmh_join();

	return true;

}


