#include <Arduino.h>
#include <SPI.h>
#include "CayenneLPP.h"

#include "utils.h"
#include "battery.h"
#include "lorawan.h"
#include "wdt.h"
#include "log.h"
#include "settings.h"
#include "sensors.h"

// ----------------------------------------------------------------------------
// Modules variables
// ----------------------------------------------------------------------------

/** Semaphore used by events to wake up loop task */
SemaphoreHandle_t taskEvent = NULL;

/** Timer to wakeup task frequently and send message */
SoftwareTimer taskWakeupTimer;

/**
 * @brief Flag for the event type
 * 0 => No event
 * 1 => LoRaWan data received
 * 2 => Timer wake up
 * 3 => Interrupt wake up
 */
uint8_t eventType = 0;

/** Message buffer */
CayenneLPP lpp(25);

// ----------------------------------------------------------------------------
// Callbacks
// ----------------------------------------------------------------------------

void periodicWakeup(TimerHandle_t unused) {
	
	// Set the event type to timer wakeup	
	if (0 == eventType) eventType = 2;
	
	// Give the semaphore, so the loop task will wake up
	xSemaphoreGiveFromISR(taskEvent, pdFALSE);

}

void interruptWakeup() {
	
	// Set the event type to timer wakeup	
	if (0 == eventType) eventType = 3;
	
	// Give the semaphore, so the loop task will wake up
	xSemaphoreGiveFromISR(taskEvent, pdFALSE);

}

void receive_callback(lorawan_message_type type, lmh_app_data_t * data) {

	switch (type) {

		case LORAWAN_TYPE_JOINED:
			logWrite(LOG_INFO, "MAIN", "Network joined");
			break;

		case LORAWAN_TYPE_JOIN_FAILED:
			logWrite(LOG_INFO, "MAIN", "OVER_THE_AIR_ACTIVATION failed!\nCheck your EUI's and Keys's!\nCheck if a Gateway is in range!");
			break;

		case LORAWAN_TYPE_UPLINK_CONF:
		case LORAWAN_TYPE_UPLINK_UNCONF:
			//logWrite(LOG_INFO, "MAIN", "Message sent");
			break;

		case LORAWAN_TYPE_DOWNLINK:
			//logWrite(LOG_INFO, "MAIN", "Downlink received: %d", data->buffer);
			if (0 == eventType) eventType = 1;
			xSemaphoreGive(taskEvent);		
			break;

		default:
			break;

	}

}

// ----------------------------------------------------------------------------
// Entrypoints
// ----------------------------------------------------------------------------

void send() {

	sensors_power(true);
	sensors_read();
    
	lpp.reset();
	lpp.addVoltage(1, battery_read() / 1000.0);

	#ifdef INTERRUPT_GPIO
	lpp.addDigitalInput(2, !digitalRead(INTERRUPT_GPIO));
	#endif

	#if SENSOR_SHTC3_ENABLE
	lpp.addTemperature(3, sensors_temperature());
	lpp.addRelativeHumidity(4, sensors_humidity());
	#endif
	#if SENSOR_SGP40_ENABLE
	lpp.addConcentration(5, sensors_voc_raw());
	lpp.addConcentration(6, sensors_voc_index());
	#endif
	#if SENSOR_MQ135_ENABLE
	if (2 == eventType) {
		lpp.addConcentration(7, sensors_nh3());
	}
	#endif

	lorawan_send(lpp.getBuffer(), lpp.getSize());

	sensors_power(false);

}

void setup() {
    
    // Create the loopTask semaphore
    taskEvent = xSemaphoreCreateBinary();
    
	// Initialize semaphore
    xSemaphoreGive(taskEvent);
	
	// Disable onboard LED
	pinMode(LED_BUILTIN, OUTPUT);
	digitalWrite(LED_BUILTIN, LOW);

	// Disable blue LED
	pinMode(LED_CONN, OUTPUT);
	digitalWrite(LED_CONN, LOW);
	
    // Setup the watchdog timer
	#ifdef WDT_SECONDS
    wdt_setup(WDT_SECONDS);
	#endif

    // Initialize serial communications and wait for port to open
    Serial.begin(115200);
	#ifdef DEVELOPMENT
    	while ((!Serial) && (millis() < 1000));
    	utils_delay(500);
	#endif

    // Setup log
    logSetup(&Serial);
    logLevel(DEBUG_LEVEL);
    logWrite(LOG_INFO, "MAIN", "%s %s", APP_NAME, APP_VERSION);

    // Set LoRa radio to low power
    lorawan_setup(receive_callback);

    // Init sensors
	sensors_power(true);
	battery_setup();
    sensors_setup();
	sensors_power(false);

	// Take the semaphore so the loop will go to sleep until an event happens
	xSemaphoreTake(taskEvent, 10);
	
	// Start the timer that will wakeup the loop frequently
    taskWakeupTimer.begin(READ_INTERVAL_SECONDS * 1000, periodicWakeup);
    taskWakeupTimer.start();

	// Interrupt wakeup
	#ifdef INTERRUPT_GPIO
		pinMode(INTERRUPT_GPIO, INPUT);
    	attachInterrupt(digitalPinToInterrupt(INTERRUPT_GPIO), interruptWakeup, INTERRUPT_TRIGGER);
	#endif

	
}

void loop() {

	static unsigned long start = 0;
	bool back_to_sleep = false;

	// Sleep until we are woken up by an event
	if (xSemaphoreTake(taskEvent, portMAX_DELAY) == pdTRUE)	{	
	
		// WDT
		#ifdef WDT_SECONDS
		wdt_feed();
		#endif
		
		// Switch on blue LED to show we are awake
		#ifdef DEVELOPMENT
			digitalWrite(LED_BUILTIN, HIGH);
		#endif
		
		// Timer
		#if SENSOR_MQ135_ENABLE
		if (2 == eventType) {
			if (0 == start) {
				start = millis();
			} else if ((millis() - start) > SENSOR_MQ135_WARMUP_MS) {
				send();
				start = 0;
				back_to_sleep = true;
			}
		}
		#else
		if (2 == eventType) {
			send();
			back_to_sleep = true;
		}
		#endif
		
		// Interrupt
		if (3 == eventType) {
			send();
			back_to_sleep = true;
		}

		if (back_to_sleep) {

			// Switch off blue LED
			#ifdef DEVELOPMENT
				digitalWrite(LED_BUILTIN, LOW);
			#endif
			
			// Go back to sleep
			eventType = 0;
			xSemaphoreTake(taskEvent, 10);
		
		}	

	}

}
