#ifndef Sensors_H
#define Sensors_H
#include <Adafruit_AHTX0.h>
#include <Adafruit_VEML7700.h>
#include <Wire.h>

extern const int soilSensorPin;
extern const int trigPin;
extern const int echoPin;

struct SensorData {
    int soilHumidity;
    float lux;
    float airHumidity;
    float temperature;
    float waterDistance;
};

void readAllSensors();

int readSensor();
int checkTemperature();
int checkLux();
float checkHumidity();
float readDistance();
void calculateDistance();
bool isAHTPresent();
bool isVEMLPresent();
void I2C_Recover();

extern SensorData globalSensors; 

#endif
