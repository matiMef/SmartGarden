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
const unsigned long screenInterval = 10000;

// watering delay
unsigned long wateringInterval = 3600000; 
unsigned long moistureCheckInterval = 600000;
unsigned long lastMoistureCheck = 0;

// ultrasonic sensor config
float distance = 0;

// low water level flag
bool lowWaterLevel = true;

// ap connection flag
bool configuration = false;
bool breakConfiguration = false;
bool fristIteration = true;
bool fristIterationDefault = false;

const char* apSsid = "";
const char* apPassword = "";

unsigned long allSensorsInterval = 10000;
unsigned long lastAllSensorsMeasurement = 0;

void createDefaultAccessPoint();
void readAllSensors();

// server config
WiFiServer server(80);

// soil profiles config
int profile = 0;
int moistureProfiles[] = {153, 127, 191}; 
int moistureProfilesAlgorithm[3][2] = { 
  { 127, 153 }, //profile 0
  { 102, 127 }, //profile 1
  { 153, 191 } //profile 2
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
    while (1) delay(10); 
  }  

  // lux sensor debugging
  if (!veml.begin()) {
    Serial.println("Nie wykryto czujnika VEML7700!");
  }
  Serial.println("Czujnik VEML7700 wykryty");

  // soil sensor debugging
  int testSoil = analogRead(soilSensorPin);
  if (testSoil < 0 || testSoil > 1023) {
    while (1) delay(10); 
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
  readAllSensors();
}

void loop() {

  if(!configuration)
  {
    if(fristIterationDefault){
      tft.fillScreen(ST77XX_BLACK);
      WiFi.disconnect();
      delay(200);
      readAllSensors();
      createDefaultAccessPoint();
      tftPrint();
      fristIterationDefault = false;
    }
    handle_watering();
    
    // pumpTest();

    if(!wateringActive){
      changeProfile();
      checkConfiguration();
      if(millis() - lastAllSensorsMeasurement >= allSensorsInterval){
        readAllSensors();
        lastAllSensorsMeasurement  = millis();
      }
      if (!configuration && !lowWaterLevel && (millis() - lastMoistureCheck >= moistureCheckInterval)){
      checkMoisture();
      lastMoistureCheck = millis();
      }
      if (millis() - lastScreenUpdate >= screenInterval) {
        tftPrint();
        lastScreenUpdate = millis();
      }
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
  if (millis() - lastWateringTime < wateringInterval ) {
    hold = true; 
    return;  
  }
  hold = false; 

  // Making more than one measurment to be sure the watering is necessary
  int i=0;
  while(i<15){
  int sensorValue = readSensor();
  if ( sensorValue <= moistureProfiles[profile] ){
    waterCount++;
  }
  if ( waterCount >= 12 ){
    watering_process();
  }
  i++;
  }
}

// This function returns soilLevel as fixed value for watering algohoritm
int checkSoilHumidity() {
  int sensorValue = globalSensors.soilHumidity;
  int soilLevel; // 0=low, 1=mid, 2=high

  if (sensorValue < moistureProfilesAlgorithm[profile][0]) soilLevel = 0;
  else if (sensorValue < moistureProfilesAlgorithm[profile][1]) soilLevel = 1;
  else soilLevel = 2;
  return soilLevel;
}

void shortWatering(){
startWatering(3000);
Serial.println("Short Watering");
}

void midWatering(){
startWatering(4000);
Serial.println("Mid Watering");
}

void longWatering(){
startWatering(5000);
Serial.println("Long Watering");
}

void watering_process() {
    byte wateringTable[3][4] = {
    // cool, opt, warm, hot
    {1, 3, 3, 2}, // low soil
    {1, 2, 2, 1}, // mid soil
    {0, 1, 0, 0}  // high soil
    };

    int soilLevel = checkSoilHumidity();
    int tempLevel = checkTemperature(); 
    int luxLevel = checkLux();
    float humidity = globalSensors.airHumidity;

    // 0=none, 1=short, 2=mid, 3=long
    int byteAction = wateringTable[soilLevel][tempLevel];

    if ((tempLevel == 2 or tempLevel == 1) and (humidity < 30) and (byteAction < 3)){
      byteAction += 1;
      Serial.println("LOW HUMIDITY, WATERING TIME EXTENDED");
    }
    else if ((tempLevel == 2 or tempLevel == 1) and (byteAction > 1) and (humidity > 75)){
      byteAction -= 1;
      Serial.println("HIGH HUMIDITY, WATERING TIME SHORTENED");
    }
    
    if ((luxLevel == 0) and (byteAction > 1)){
      byteAction -= 1;
      Serial.println("LOW LUX, WATERING TIME SHORTENED");
    }
    
    if ( byteAction == 1) shortWatering();
    else if ( byteAction == 2) midWatering();
    else if ( byteAction == 3) longWatering();
    else if ( byteAction == 0) lastWateringTime = millis();
  
    waterCount = 0;
}

void pumpTest(){
  digitalWrite(relayPin, LOW);
  delay(5000);
  digitalWrite(relayPin, HIGH);
  Serial.println("TEST 5s OK");
  delay(500);
  digitalWrite(relayPin, LOW);
  delay(10000);
  digitalWrite(relayPin, HIGH);
  Serial.println("TEST 10s OK");
  delay(500);
  digitalWrite(relayPin, LOW);
  delay(15000);
  digitalWrite(relayPin, HIGH);
  Serial.println("TEST 15s OK");
  delay(500);
  digitalWrite(relayPin, LOW);
  delay(20000);
  digitalWrite(relayPin, HIGH);
  Serial.println("TEST 20s OK");
  delay(500);
  digitalWrite(relayPin, LOW);
  delay(25000);
  digitalWrite(relayPin, HIGH);
  Serial.println("TEST 25s OK");
  delay(99999);
  Serial.println("KONIEC TESTU");
}

// This function starts watering and saves time of watering begin
void startWatering(unsigned long duration){
  Serial.println("WATERING START");
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
  if (wateringActive && (millis() - wateringStart >= wateringDuration)) {
    digitalWrite(relayPin, HIGH);  // pump off
    wateringActive = false;
    lastWateringTime = millis(); 
    Serial.println("WATERING STOP");
    delay(500);
  }
}

