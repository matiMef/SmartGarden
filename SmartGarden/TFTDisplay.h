#ifndef TFTDISPLAY_H
#define TFTDISPLAY_H

#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <Adafruit_AHTX0.h>
#include <Adafruit_VEML7700.h>
#include <SPI.h>

void tftPrint();
void tftSoil();
void tftHumTemp();
void tftLux();
void tftWaterLevel();
void tftProfile();
void drawWiFiIcon(int x, int y, uint16_t color);
void confTft();

#endif