#include <SPI.h>
#include "MQ135.h"

//#define RZERO 76.63
#define RZERO 16.28
#define RLOAD 5.0

MQ135 mq135_sensor(WB_A0, RZERO, RLOAD);
float temperature = 18.0;
float humidity = 50.0;

void setup() {
    Serial.begin(115200);
    analogReference(AR_INTERNAL_3_0);
    analogReadResolution(12);
}

void loop() {

  float rzero = mq135_sensor.getRZero(ATMONH4);
  float correctedRZero = mq135_sensor.getCorrectedRZero(ATMONH4, temperature, humidity);
  float resistance = mq135_sensor.getResistance();
  float ppm = mq135_sensor.getPPM();
  float correctedPPM = mq135_sensor.getCorrectedPPM(temperature, humidity);

  float analog= analogRead(WB_A0);

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

