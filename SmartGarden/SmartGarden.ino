#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_AHTX0.h>
#include <Adafruit_VEML7700.h>
#include <EEPROM.h>
#include <WiFiS3.h>
#include "Wifi.h"
#include "TFTDisplay.h"
#include "Sensors.h"
#include "Profile.h"
#include "WifiServer.h"

// tft
#define TFT_CS     10
#define TFT_DC      9
#define TFT_RST     8

// soil humidity sensor 
const int soilSensorPin = A0;

//  ultrasonic sensor
const int trigPin = 2;
const int echoPin = 3;

// button profile
const int buttonPin = 4;
const int buttonConfigurationPin = 6;

// tft declaration
Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

// humTemp sensor declaration
Adafruit_AHTX0 aht;

// lux sensor declaration
Adafruit_VEML7700 veml = Adafruit_VEML7700();

// relay pin declaration
const int relayPin = 7;

// watering config
int waterCount = 0;
unsigned long lastWateringTime = 0;
unsigned long WateringTime = 0;
unsigned long wateringStart = 0;
unsigned long wateringDuration = 0;

// watering alghoritm flags
bool hold = true;
bool wateringActive = false;

// tft refresh interval
unsigned long lastScreenUpdate = 0;
const unsigned long screenInterval = 3000;

// watering delay
unsigned long wateringInterval = 600000;

// ultrasonic sensor config
float distance = 0;

// low water level flag
bool lowWaterLevel = false;

// ap connection flag
bool configuration = false;
bool breakConfiguration = false;
bool fristIteration = true;
bool fristIterationDefault = false;

const char* apSsid = "";
const char* apPassword = "";

void createDefaultAccessPoint();

// server config
WiFiServer server(80);

// soil profiles config
int profile = 0;
int moistureProfiles[] = {128, 102, 153}; 
int moistureProfilesAlgorithm[3][2] = { 
  { 77, 128 }, //profile 0
  { 51, 102 }, //profile 1
  { 102, 153 } //profile 2
};

void setup() {

  // cli
  Serial.begin(9600);

  // delaying for peripherials start
  delay(1500);

  // profile init 
  profile = EEPROM.read(0);
  // profile troubleshooting
  if (profile < 0 || profile > 2) profile = 0;

  // relay init
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, HIGH);

  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(buttonConfigurationPin, INPUT_PULLUP);

  // humtemp sensor debugging
  if (!aht.begin()) {
    Serial.println("Nie znaleziono AHT10");
    while (1) delay(10); // zatrzymanie programu jeśli czujnik nie działa
  }  

  // lux sensor debugging
  if (!veml.begin()) {
    Serial.println("Nie wykryto czujnika VEML7700!");
    while (1);
  }
  Serial.println("Czujnik VEML7700 wykryty");

  // soil sensor debugging
  int testSoil = analogRead(soilSensorPin);
  if (testSoil < 0 || testSoil > 1023) {
    Serial.println("Problem z czujnikiem wilgotności gleby!");
  } else {
    Serial.println("Czujnik gleby działa.");
  }

  // tft init
  tft.init(240, 320);
  tft.setRotation(1);  
  tft.fillScreen(ST77XX_BLACK);

  // lux sensor init
  veml.setGain(VEML7700_GAIN_1);  
  veml.setIntegrationTime(VEML7700_IT_100MS);  
  
  // ultrasonic sensor init
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  createDefaultAccessPoint();
  
  server.begin();
}

void loop() {

  if(!configuration)
  {
    if(fristIterationDefault){
      tft.fillScreen(ST77XX_BLACK);
      WiFi.disconnect();
      delay(200);
      createDefaultAccessPoint();
      tftPrint();
      fristIterationDefault = false;
    }
    handle_watering();
    changeProfile(); 
    checkConfiguration();
    calculateDistance();
    
    if (!wateringActive && !lowWaterLevel ){
      checkMoisture();
    };

    if (millis() - lastScreenUpdate >= screenInterval) {
      tftPrint();
      lastScreenUpdate = millis();
    }
  
    sendData();
  }

  if(configuration){
    if(fristIteration){
      fristIteration = false;
      tft.fillScreen(ST77XX_BLACK);
      WiFi.disconnect();
      delay(200);
      confTft();
    }
    checkConfiguration();
    createAccessPoint();
    if (millis() - lastScreenUpdate >= screenInterval) {
      confTft();
      lastScreenUpdate = millis();
    }
    if(!configuration && !breakConfiguration){
        WiFi.disconnect();
        tft.fillScreen(ST77XX_BLACK);
        createDefaultAccessPoint();
      }
  }
}

void checkConfiguration(){
  if (digitalRead(buttonConfigurationPin) == 0){
    delay(100);
    if (!configuration){
      configuration = true;
      breakConfiguration = false;
      fristIteration = true;
      delay(100);
    }
    else{
      configuration = false;
      breakConfiguration = true;
      fristIterationDefault = true;
      delay(100);
    }
  }
}

// This function is responsible for starting watering alghoritm in estabilished time
void checkMoisture() {

  //  Watering time suspension
  if (millis() - lastWateringTime < wateringInterval) {
    hold = true; 
    return;  
  }
  hold = false; 

  // Making more than one measurment to be sure the watering is necessary 
  int sensorValue = readSensor();
  if ( sensorValue <= moistureProfiles[profile] ){
    waterCount++;
  }
  if ( waterCount >= 10 ){
    watering_process();
  }
}

// This function returns soilLevel as fixed value for watering algohoritm
int checkSoilHumidity() {
  int sensorValue = readSensor();
  int soilLevel; // 0=low, 1=mid, 2=high

  if (sensorValue < moistureProfilesAlgorithm[profile][0]) soilLevel = 0;
  else if (sensorValue < moistureProfilesAlgorithm[profile][1]) soilLevel = 1;
  else soilLevel = 2;
  return soilLevel;
}

void shortWatering(){
start_watering(3000);
}

void midWatering(){
start_watering(5000);
}

void longWatering(){
start_watering(10000);
}

void watering_process() {
    byte wateringTable[3][4] = {
    // cool, opt, warm, hot
    {1, 3, 3, 2}, // low soil
    {1, 2, 2, 1}, // mid soil
    {0, 1, 1, 0}  // high soil
    };

    int soilLevel = checkSoilHumidity();
    int tempLevel = checkTemperature(); 
    int luxLevel = checkLux();
    float humidity = checkHumidity();

    // 0=none, 1=short, 2=mid, 3=long
    int byteAction = wateringTable[soilLevel][tempLevel];

    if ( (tempLevel == 3 or tempLevel == 2) and humidity < 30){
      byteAction += 1;
    }
    else if ( (tempLevel == 3 or tempLevel == 2) and ( luxLevel == 0 ) and (byteAction > 0) and (humidity > 75 )){
      byteAction -= 1;
    }
    
    if ( byteAction == 1) shortWatering();
    else if ( byteAction == 2) midWatering();
    else if ( byteAction == 3) longWatering();

    waterCount = 0;
}

// This function starts watering and saves time of watering begin
void start_watering(unsigned long duration){
  digitalWrite(relayPin, LOW);   // pump on
  wateringStart = millis();
  wateringDuration = duration;
  wateringActive = true;
}

// This function breaks watering
void handle_watering() {
  if (hold == true){
    return;
  };
  if (wateringActive == true && (millis() - wateringStart >= wateringDuration)) {
    digitalWrite(relayPin, HIGH);  // pump off
    wateringActive = false;
    lastWateringTime = millis();   
  }
}

