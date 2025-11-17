#ifndef Sensors_H
#define Sensors_H
#include <Adafruit_AHTX0.h>
#include <Adafruit_VEML7700.h>
#include <Wire.h>

extern const int soilSensorPin;
extern const int trigPin;
extern const int echoPin;

int readSensor();
int checkTemperature();
int checkLux();
float checkHumidity();
float readDistance();
void calculateDistance();

#endif
