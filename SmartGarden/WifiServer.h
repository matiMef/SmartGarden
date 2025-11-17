#ifndef WIFI_SERVER_H
#define WIFI_SERVER_H

#include <Adafruit_AHTX0.h>
#include <Adafruit_VEML7700.h>
#include <WiFiS3.h>

void sendData();
void createAccessPoint();
String readStringFromEEPROM(int addrOffset);
int readStringLenght(int addrOffset);

#endif