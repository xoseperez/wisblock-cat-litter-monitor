#include <SPI.h>
#include "MQ135.h"
#include "ADC_SGM58031.h"

//#define RZERO 76.63
#define RZERO 27.51
#define RLOAD 5.0

MQ135 mq135_sensor(0, RZERO, RLOAD);
RAK_ADC_SGM58031 sgm58031(SGM58031_SDA_ADDRESS);
float temperature = 25.0;
float humidity = 70.0;

void setup() {
    Serial.begin(115200);
    analogReference(AR_INTERNAL_3_0);
    analogReadResolution(12);
    pinMode(WB_IO2, OUTPUT);
    digitalWrite(WB_IO2, HIGH);
    delay(300);
		sgm58031.begin();
		sgm58031.setConfig(0xC2E0);

   
}

void loop() {

  uint16_t analog = sgm58031.getAdcValue() >> 3;
  mq135_sensor.setReading(analog);
  float rzero = mq135_sensor.getRZero(ATMONH4);
  float correctedRZero = mq135_sensor.getCorrectedRZero(ATMONH4, temperature, humidity);
  float resistance = mq135_sensor.getResistance();
  float ppm = mq135_sensor.getPPM();
  float correctedPPM = mq135_sensor.getCorrectedPPM(temperature, humidity);

  Serial.print("Reading: ");
  Serial.print(analog);
  Serial.print("\tVoltage: ");
  Serial.print(analog * 3000 / 4096.0);
  Serial.print("\tMQ135 RZero: ");
  Serial.print(rzero);
  Serial.print("\t Corrected RZero: ");
  Serial.print(correctedRZero);
  Serial.print("\t Resistance: ");
  Serial.print(resistance);
  Serial.print("\t PPM: ");
  Serial.print(ppm);
  Serial.print("\t Corrected PPM: ");
  Serial.print(correctedPPM);
  Serial.println("ppm");

  delay(5000);

}

