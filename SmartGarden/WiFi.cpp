#include "Wifi.h"

extern bool connected;
extern bool configuration;

extern const char* apSsid;
extern const char* apPassword;

void drawWiFiIcon(int x, int y, uint16_t color);

String readStringFromEEPROM(int addrOffset);
int readStringLenght(int addrOffset);
IPAddress ip(192, 168, 4, 1);

// Attempting connection
void createDefaultAccessPoint(){
  drawWiFiIcon(310, 10, ST77XX_YELLOW);

  WiFi.config(ip);

  String ssidBuffer = readStringFromEEPROM(1);
  apSsid = ssidBuffer.c_str();
  int passwordOffset = readStringLenght(1)+2;
  String passwordBuffer = readStringFromEEPROM(passwordOffset);
  apPassword = passwordBuffer.c_str();

  WiFi.beginAP(apSsid, apPassword);
}

void checkConnection(){
String defaultIp = "192.168.4.1";
String configurationIp = "192.168.5.1";
String currentIpAddress = WiFi.localIP().toString();

if(currentIpAddress == defaultIp){
  drawWiFiIcon(310, 10, ST77XX_GREEN);
}
else if(currentIpAddress == configurationIp){
  drawWiFiIcon(310, 10, ST77XX_BLUE);
}
else{
  drawWiFiIcon(310, 10, ST77XX_RED);
}
}







