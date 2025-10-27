#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_AHTX0.h>
#include <Adafruit_VEML7700.h>
#include <EEPROM.h>
#include <WiFiS3.h>     // dla Arduino UNO R4 WiFi (dla ESP32: <WiFi.h>)
#include "TFTDisplay.h"

// soil humidity sensor 
#define sensorPin A0
#define buttonPin 4

// tft
#define TFT_CS     10
#define TFT_DC      9
#define TFT_RST     8

//  ultrasonic sensor
#define TRIG_PIN 2
#define ECHO_PIN 3

// tft declaration
Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

// humTemp sensor declaration
Adafruit_AHTX0 aht;

// lux sensor declaration
Adafruit_VEML7700 veml = Adafruit_VEML7700();

// relay pin declaration
int relayPin = 7;

// watering config
int water_count = 0;
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

// measurment delay
unsigned long lastMeasurement = 0;
const unsigned long measureInterval = 2000; // co ile ms nowy pomiar

// ultrasonic sensor config
float distance = 0;

// low water level flag
bool lowWaterLevel = false;

// wifi interval
unsigned long findWifiInterval = 30000;
unsigned long lastListening = -30000;

// wifi config
const char* ssid = "trojan.exe";
const char* password = "52607127";

// wifi connection flag
bool connected = false;

// server config
WiFiServer server(80);

// soil profiles config
int profile = 0;
int moistureProfiles[] = {128, 102, 153}; 
int moistureProfilesAlgorithm[3][2] = { { 77, 128 }, //profile 0
                             { 51, 102 }, //profile 1
                             { 102, 153 } //profile2
};

void setup() {

  // cli
  Serial.begin(9600);

  // delaying for peripherials start
  delay(1500);

  // wifi debugging
  WiFi.begin(ssid, password);

   while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nPołączono z Wi-Fi");
  Serial.print("Adres IP: ");
  Serial.println(WiFi.localIP());

  // profile init 
  profile = EEPROM.read(0);
  // profile troubleshooting
  if (profile < 0 || profile > 2) profile = 0;

  // relay init
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, HIGH);

  pinMode(buttonPin, INPUT_PULLUP);

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
  int testSoil = analogRead(sensorPin);
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
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  // connecting to the Network
  connectToTheNetwork();

  // checking connection to update icon instantly
  checkConnection();

  // HTTP server init
  server.begin();
}

void loop() {
  changeProfile();
  handle_watering();
  calculateDistance();
   
  if (!wateringActive && !lowWaterLevel ){
    checkMoisture();
  };

  if (millis() - lastScreenUpdate >= screenInterval) {
    tftPrint();
    lastScreenUpdate = millis();
  }

  checkConnection();

   if ( (millis() - lastListening > findWifiInterval ) && !connected){
    connectToTheNetwork();
    lastListening = millis();
  }
  
  sendData();
}

void sendData(){
   WiFiClient client = server.available();
  if (client) 
  {
    if (client.available()) 
    {
      String req = client.readStringUntil('\r');
      client.flush();

        sensors_event_t humidityEvent, tempEvent;
        aht.getEvent(&humidityEvent, &tempEvent);
        float temp = tempEvent.temperature;
        float humidity = humidityEvent.relative_humidity;
        float lux = veml.readLux();
        int soil = readSensor();
        float distance = readDistance();

      if (req.indexOf("GET /data") != -1) {
        String json = "{";
        json += "\"temperature\":" + String(temp, 2) + ",";
        json += "\"humidity\":" + String(humidity, 2) + ",";
        json += "\"soil\":" + String(soil) + ",";
        json += "\"lux\":" + String(lux) + ",";
        json += "\"distance\":" + String(distance);
        json += "}";

        client.println("HTTP/1.1 200 OK");
        client.println("Content-Type: application/json");
        client.println("Connection: close");
        client.println();
        client.println(json);
      }

      if (req.indexOf("POST /command") != -1) 
      {
        while (client.available()) {
        String body = client.readStringUntil('\n');
        if (body.length() == 0) break; // koniec nagłówków
      }

      String payload = client.readString();

      if (payload.indexOf("start_short") != -1) 
      {
        hold = false;
        short_watering();
      }
      else if(payload.indexOf("start_mid") != -1)
      {
        hold = false;
        mid_watering();
      }
      else if(payload.indexOf("start_long") != -1)
      {
        hold = false;
        long_watering();
      }
        client.println("HTTP/1.1 200 OK");
        client.println("Content-Type: text/plain");
        client.println("Connection: close");
        client.println(); 
        client.println("OK");
      }
    }
    
    delay(1);
    client.stop();
  }
}

void checkConnection(){
  if (WiFi.status() == WL_CONNECTED){
    // Serial.print("Połączono z Wifi");
    connected = true;
  }
  else{
    connected = false;
  }
  changeWifiIconColor();
}

void start_watering(unsigned long duration){
  digitalWrite(relayPin, LOW);   // włącz pompę
  wateringStart = millis();
  wateringDuration = duration;
  wateringActive = true;
}

void handle_watering() {
  if (hold == true){
    return;
  };
  // Serial.println((millis() - wateringStart));
  // Serial.println(wateringDuration);
  if (wateringActive == true && (millis() - wateringStart >= wateringDuration)) {
    digitalWrite(relayPin, HIGH);  // wyłącz pompę
    wateringActive = false;
    lastWateringTime = millis();   // zapamiętaj kiedy skończyłeś podlewać
  }
}

void short_watering(){
start_watering(3000);
}

void mid_watering(){
start_watering(5000);
}

void long_watering(){
start_watering(10000);
}

//  This function returns the analog data to calling function
int readSensor() {
  int sensorValue = analogRead(sensorPin);  
  int outputValue = map(sensorValue, 0, 1023, 255, 0); // map the 10-bit data to 8-bit data
  return outputValue;             
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
    water_count++;
  }
  if ( water_count >= 10 ){
    watering_process();
  }
}

int checkTemperature() {
  sensors_event_t humidityObject, tempObject;
  aht.getEvent(&humidityObject, &tempObject);
  float temp = tempObject.temperature;
  int tempLevel; // 0=zimno, 1=optymalnie, 2=gorąco

  if (temp < 15) tempLevel = 0;
  else if (temp < 25) tempLevel = 1;
  else if (temp < 35) tempLevel = 2;
  else tempLevel = 3;
  
  return tempLevel;
}

int checkSoilHumidity() {
  int sensorValue = readSensor();
  int soilLevel; // 0=niska, 1=średnia, 2=wysoka

  if (sensorValue < moistureProfilesAlgorithm[profile][0]) soilLevel = 0;
  else if (sensorValue < moistureProfilesAlgorithm[profile][1]) soilLevel = 1;
  else soilLevel = 2;
  return soilLevel;
}

int checkLux(){
  float lux = veml.readLux();
  int luxLevel;
  if ( lux<10 ) luxLevel = 0;
  else luxLevel = 1;
  
  return luxLevel;
}

float checkHumidity(){
  sensors_event_t humidityObject, tempObject;
  aht.getEvent(&humidityObject, &tempObject);
  float humidity = humidityObject.relative_humidity;

  return humidity;
}

void setProfile(int newProfile) {
  profile = newProfile;
  EEPROM.write(0, profile); // zapisz w pamięci
}

void changeProfile() {
  int newProfile;
  if ( digitalRead(buttonPin) == 0){
    if ( profile == 0 || profile == 1){
      newProfile = profile + 1;
      setProfile(newProfile);
      tftPrint();
    } 
    else{
      newProfile = 0;
      setProfile(newProfile);
      tftPrint();
    }
  };
}

float readDistance() {
  // wyślij impuls
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  // zmierz czas
  long duration = pulseIn(ECHO_PIN, HIGH, 30000); // timeout 30 ms
  if (duration == 0) return -1; // brak odczytu

  return duration * 0.0343 / 2.0;
  delay(50);
}

void calculateDistance(){
   if (millis() - lastMeasurement >= measureInterval) {
    lastMeasurement = millis();
    distance = readDistance();
    Serial.println(lowWaterLevel);
    if ( distance > 10 ){
      lowWaterLevel = true;
    }
    else{
      lowWaterLevel = false;
    }
  }
}

void connectToTheNetwork(){
  drawWiFiIcon(310, 10, ST77XX_YELLOW);
  WiFi.begin(ssid, password);
}

void watering_process() {
    byte wateringTable[3][4] = {
    {1, 3, 2, 1}, // niska gleba: zimno=short(1), opt=long(2), ciepło=mid(3), gorąco=short(4)
    {1, 2, 1, 0}, // średnia gleba
    {0, 1, 1, 0}  // wysoka gleba
    };

    int soilLevel = checkSoilHumidity();
    int tempLevel = checkTemperature(); 
    int luxLevel = checkLux();
    float humidity = checkHumidity();

    // interpretacja: 0=none, 1=short, 2=mid, 3=long
    int byteAction = wateringTable[soilLevel][tempLevel];

    if ( (tempLevel == 3 or tempLevel == 2) and humidity < 30){
      byteAction += 1;
    }
    else if ( soilLevel == 2 and byteAction > 0 and humidity > 75 ){
      byteAction -= 1;
    }
    else if ( (tempLevel == 3 or tempLevel == 2) and ( luxLevel == 0 ) and byteAction > 0 ){
      byteAction -= 1;
    }
    
    if ( byteAction == 1) short_watering();
    else if ( byteAction == 2) mid_watering();
    else if ( byteAction == 3) long_watering();

    water_count = 0;
}
