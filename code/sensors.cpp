#include <Arduino.h>
#include <Wire.h>
#include "Adafruit_SHTC3.h"
#include "Adafruit_SGP40.h" 

#include "sensors.h"
#include "log.h"
#include "utils.h"
#include "settings.h"

// ----------------------------------------------------------------------------
// Private variables
// ----------------------------------------------------------------------------

uint16_t _sensors_voc_raw = 0;
int32_t _sensors_voc_index = 0;
float _sensors_temperature = 0;
uint8_t _sensors_humidity = 0;

Adafruit_SGP40 _sensors_sgp40;
Adafruit_SHTC3 _sensors_shtc3 = Adafruit_SHTC3();
bool _sensors_shtc3_ready = false;
bool _sensors_sgp40_ready = false;

// ----------------------------------------------------------------------------
// Public methods
// ----------------------------------------------------------------------------

void sensors_power(bool power) {
    
	logWrite(LOG_DEBUG, "SNS", "Sensor power %s", power ? "ON" : "OFF");
	
	// Turn on/off current 
	pinMode(SENSOR_POWER_GPIO, OUTPUT);
	digitalWrite(SENSOR_POWER_GPIO, power ? HIGH : LOW);

	if (_sensors_shtc3_ready) _sensors_shtc3.sleep(!power);
	if (power) _sensors_sgp40_ready = _sensors_sgp40.begin();

	utils_delay(100);

}

uint16_t sensors_voc_raw() { return _sensors_voc_raw; }
int32_t sensors_voc_index() { return _sensors_voc_index; }
float sensors_temperature() { return _sensors_temperature; }
uint8_t sensors_humidity() { return _sensors_humidity; }

void sensors_read() {

	// reset
	_sensors_voc_raw = 0;
	_sensors_voc_index = 0;
	_sensors_temperature = 0;
	_sensors_humidity = 0;

	// Readh SHTC3
	if (_sensors_shtc3_ready) {
		sensors_event_t h, t;
  		_sensors_shtc3.getEvent(&h, &t);
		_sensors_temperature = t.temperature;
		_sensors_humidity = h.relative_humidity;
		logWrite(LOG_INFO, "SNS", "Temperature: %.1f", _sensors_temperature);
		logWrite(LOG_INFO, "SNS", "Humidity: %d%%", _sensors_humidity);
	}
  
	// Read SGH40
	if (_sensors_shtc3_ready & _sensors_sgp40_ready) {
		_sensors_voc_raw = _sensors_sgp40.measureRaw(_sensors_temperature, _sensors_humidity);
		_sensors_voc_index = _sensors_sgp40.measureVocIndex(_sensors_temperature, _sensors_humidity);
		logWrite(LOG_INFO, "SNS", "VOC (raw): %d", _sensors_voc_raw);
		logWrite(LOG_INFO, "SNS", "VOC (index): %d", _sensors_voc_index);
	}

}

bool sensors_setup() {

	// Sensirion SHTC3
  	_sensors_shtc3_ready = _sensors_shtc3.begin();
	if (! _sensors_shtc3_ready) {
	    logWrite(LOG_ERROR, "SNS", "SHTC3 init failed");
  	}

	// Sensirion SGH40
	_sensors_sgp40_ready = _sensors_sgp40.begin();
	if (!_sensors_sgp40_ready) {
		logWrite(LOG_ERROR, "SNS", "SGH40 init failed");
	} else {
		logWrite(LOG_INFO, "SNS", "SGH40 serial: %04X%04X%04X", _sensors_sgp40.serialnumber[0], _sensors_sgp40.serialnumber[1], _sensors_sgp40.serialnumber[2]);
	}

	return _sensors_shtc3_ready & _sensors_sgp40_ready;

}