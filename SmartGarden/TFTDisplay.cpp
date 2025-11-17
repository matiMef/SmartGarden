#include "TFTDisplay.h"

extern float distance;
extern int profile;
extern Adafruit_ST7789 tft;
extern Adafruit_AHTX0 aht;
extern Adafruit_VEML7700 veml;
extern int moistureProfilesAlgorithm[3][2];
extern String apIp;
extern void checkConnection();

int readSensor();

void tftPrint(){
  tftSoil();
  tftHumTemp();
  tftLux();
  tftWaterLevel();
  tftProfile(); 
  checkConnection();
}

void tftSoil(){
  float soil_humidity = readSensor();
  // tft display of soil sensor
  tft.fillRect(0, 0, 300, 24, ST77XX_BLACK);
  tft.setCursor(0, 0);
  tft.setTextSize(2); 
  tft.setTextColor(ST77XX_GREEN, ST77XX_BLACK);
  tft.print("Soil H: ");
  if ( soil_humidity < moistureProfilesAlgorithm[profile][0]){
    tft.setTextColor(ST77XX_RED, ST77XX_BLACK);
    tft.print("Low");
  }
  else if ( soil_humidity > moistureProfilesAlgorithm[profile][0] && soil_humidity < moistureProfilesAlgorithm[profile][1]){
    tft.setTextColor(ST77XX_YELLOW, ST77XX_BLACK);
    tft.print("Mid");
  }
  else{
    tft.setTextColor(ST77XX_GREEN, ST77XX_BLACK);
    tft.print("High");
  }
  tft.setTextColor(ST77XX_GREEN, ST77XX_BLACK);
}

void tftLux(){
  // ambient light sensor
  float lux = veml.readLux();
  
   // tft display lux
  tft.fillRect(0, 120, 320, 24, ST77XX_BLACK);
  tft.setCursor(0, 120);
  tft.print("Lux: ");
  if ( lux < 10 ){
    tft.setTextColor(ST77XX_RED, ST77XX_BLACK);
  }
  else if ( lux < 20 ){
    tft.setTextColor(ST77XX_YELLOW, ST77XX_BLACK);
  }
  else{
    tft.setTextColor(ST77XX_GREEN, ST77XX_BLACK);
  }
  tft.print(lux);
  tft.setTextColor(ST77XX_GREEN, ST77XX_BLACK);
}

void tftHumTemp(){
  // temp and humidity sensor 
  sensors_event_t humidity, temp;
  aht.getEvent(&humidity, &temp);

   // tft display humidity
  tft.fillRect(0, 40, 320, 24, ST77XX_BLACK);  // osobny prostok�t, �eby nadpisa� stare dane
  tft.setCursor(0, 40);
  tft.print("Air H: ");
  tft.print(humidity.relative_humidity);

  // tft display temperature
  tft.fillRect(0, 80, 320, 24, ST77XX_BLACK);
  tft.setCursor(0, 80);
  tft.print("Temp: ");
  tft.print(temp.temperature);
}

void tftProfile(){
  tft.fillRect(0, 200, 320, 24, ST77XX_BLACK);
  tft.setTextSize(2); 
  tft.setCursor(0, 200);
  tft.print("Profile: ");
  if (profile == 0) {
     tft.setTextColor(ST77XX_GREEN, ST77XX_BLACK);
     tft.print("Default");
  }
  else if (profile == 1 ){
      tft.setTextColor(ST77XX_YELLOW, ST77XX_BLACK);
     tft.print("Low Soil");
  }
  else{
      tft.setTextColor(ST77XX_BLUE, ST77XX_BLACK);
     tft.print("High Soil"); 
  }
  tft.setTextColor(ST77XX_GREEN, ST77XX_BLACK);
}

void tftWaterLevel(){
  //dane do dostosowania
  tft.fillRect(0, 160, 320, 24, ST77XX_BLACK);
  tft.setCursor(0, 160);
  tft.print("Water Level: ");
  if ( distance > 10){
    tft.setTextColor(ST77XX_RED, ST77XX_BLACK);
    tft.print("Low");
  }
  else if ( distance < 10 and distance > 5){
    tft.setTextColor(ST77XX_YELLOW, ST77XX_BLACK);
    tft.print("Mid");
  }
  else{
    tft.setTextColor(ST77XX_GREEN, ST77XX_BLACK);
    tft.println("High");
  }
  tft.setTextColor(ST77XX_GREEN, ST77XX_BLACK);
}

void drawWiFiIcon(int x, int y, uint16_t color) {
  tft.drawCircle(x, y, 3, color);
  tft.drawCircle(x, y, 6, color);
  tft.drawCircle(x, y, 9, color);
  tft.fillCircle(x, y, 2, color);
}

void confTft(){
  tft.fillRect(0, 0, 300, 24, ST77XX_BLACK);
  tft.setCursor(0, 0);
  tft.setTextSize(2); 
  tft.setTextColor(ST77XX_BLUE, ST77XX_BLACK);
  tft.print("WiFi Configuration");
  tft.fillRect(0, 24, 320, 24, ST77XX_BLACK);
  tft.setCursor(0, 24);
  tft.print("IP Address: ");
  Serial.println(apIp);
  tft.print(apIp);
  tft.fillRect(0, 60, 320, 24, ST77XX_BLACK);
  tft.setCursor(0, 60);
  tft.print("Connect to the described");
  tft.fillRect(0, 84, 320, 24, ST77XX_BLACK);
  tft.setCursor(0, 84);
  tft.print("address to enter new WiFi");
  tft.fillRect(0, 108, 320, 24, ST77XX_BLACK);
  tft.setCursor(0, 108);
  tft.print("login details.");
  checkConnection();
}

