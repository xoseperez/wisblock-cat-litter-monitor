#include <Arduino.h>
#include <Wire.h>

#include "sensors.h"
#include "log.h"
#include "utils.h"
#include "settings.h"

#if SENSOR_SHTC3_ENABLE
#include "Adafruit_SHTC3.h"
#endif

#if SENSOR_SGP40_ENABLE
#include "Adafruit_SGP40.h" 
#endif

#if SENSOR_MQ135_ENABLE
#include "MQ135.h"
#endif

// ----------------------------------------------------------------------------
// Private variables
// ----------------------------------------------------------------------------

#if SENSOR_SHTC3_ENABLE
float _sensors_temperature = 0;
uint8_t _sensors_humidity = 0;
Adafruit_SHTC3 _sensors_shtc3 = Adafruit_SHTC3();
bool _sensors_shtc3_ready = false;
#endif

#if SENSOR_SGP40_ENABLE
uint16_t _sensors_voc_raw = 0;
int32_t _sensors_voc_index = 0;
Adafruit_SGP40 _sensors_sgp40;
bool _sensors_sgp40_ready = false;
#endif

#if SENSOR_MQ135_ENABLE
uint16_t _sensors_nh3 = 0;
MQ135 _sensors_mq135(SENSOR_MQ135_GPIO, SENSOR_MQ135_RZERO, SENSOR_MQ135_RLOAD);
#endif


// ----------------------------------------------------------------------------
// Public methods
// ----------------------------------------------------------------------------

void sensors_power(bool power) {
	static bool state = false;
	if (power == state) return;
	state = power;
	logWrite(LOG_DEBUG, "SNS", "Sensor power %s", state ? "ON" : "OFF");
	pinMode(SENSOR_POWER_GPIO, OUTPUT);
	digitalWrite(SENSOR_POWER_GPIO, state ? HIGH : LOW);
}

void sensors_sleep(bool sleep) {
    
	logWrite(LOG_DEBUG, "SNS", "Sensors %s", sleep ? "sleeping" : "awake");
	
	#if SENSOR_SHTC3_ENABLE
	if (_sensors_shtc3_ready) _sensors_shtc3.sleep(sleep);
	#endif

	#if SENSOR_SGP40_ENABLE
	if (!sleep) _sensors_sgp40_ready = _sensors_sgp40.begin();
	#endif

	#if SENSOR_MQ135_ENABLE
	// Nothing to do
	#endif

	utils_delay(100);

}

#if SENSOR_SHTC3_ENABLE
float sensors_temperature() { return _sensors_temperature; }
uint8_t sensors_humidity() { return _sensors_humidity; }
#endif

#if SENSOR_SGP40_ENABLE
uint16_t sensors_voc_raw() { return _sensors_voc_raw; }
int32_t sensors_voc_index() { return _sensors_voc_index; }
#endif

#if SENSOR_MQ135_ENABLE
uint16_t sensors_nh3() { return _sensors_nh3; }
#endif

void sensors_read() {

	#if SENSOR_SHTC3_ENABLE
		_sensors_temperature = 0;
		_sensors_humidity = 0;
		if (_sensors_shtc3_ready) {
			sensors_event_t h, t;
			_sensors_shtc3.getEvent(&h, &t);
			_sensors_temperature = t.temperature;
			_sensors_humidity = h.relative_humidity;
			logWrite(LOG_INFO, "SNS", "Temperature: %.1f", _sensors_temperature);
			logWrite(LOG_INFO, "SNS", "Humidity: %d%%", _sensors_humidity);
		}
	#endif
  
	#if SENSOR_SGP40_ENABLE
		_sensors_voc_raw = 0;
		_sensors_voc_index = 0;
		if (_sensors_shtc3_ready & _sensors_sgp40_ready) {
			_sensors_voc_raw = _sensors_sgp40.measureRaw(_sensors_temperature, _sensors_humidity);
			_sensors_voc_index = _sensors_sgp40.measureVocIndex(_sensors_temperature, _sensors_humidity);
			logWrite(LOG_INFO, "SNS", "VOC (raw): %d", _sensors_voc_raw);
			logWrite(LOG_INFO, "SNS", "VOC (index): %d", _sensors_voc_index);
		}
	#endif

	#if SENSOR_MQ135_ENABLE
		#if SENSOR_SHTC3_ENABLE
			if (_sensors_shtc3_ready) {
				_sensors_nh3 = round(_sensors_mq135.getCorrectedPPM(_sensors_temperature, _sensors_humidity));
			} else {
				_sensors_nh3 = round(_sensors_mq135.getPPM());
			}
		#else
			_sensors_nh3 = round(_sensors_mq135.getPPM());
		#endif
	#endif

}

bool sensors_setup() {

	bool all_ready = true;

	#if SENSOR_SHTC3_ENABLE
		_sensors_shtc3_ready = _sensors_shtc3.begin();
		all_ready &= _sensors_shtc3_ready;
		if (! _sensors_shtc3_ready) {
			logWrite(LOG_ERROR, "SNS", "SHTC3 init failed");
		}
	#endif

	#if SENSOR_SGP40_ENABLE
		_sensors_sgp40_ready = _sensors_sgp40.begin();
		all_ready &= _sensors_sgp40_ready;
		if (!_sensors_sgp40_ready) {
			logWrite(LOG_ERROR, "SNS", "SGH40 init failed");
		} else {
			logWrite(LOG_INFO, "SNS", "SGH40 serial: %04X%04X%04X", _sensors_sgp40.serialnumber[0], _sensors_sgp40.serialnumber[1], _sensors_sgp40.serialnumber[2]);
		}
	#endif

	#if SENSOR_MQ135_ENABLE
		// Nothing to do
	#endif

	return all_ready;

}