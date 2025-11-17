#include "Sensors.h"

extern Adafruit_AHTX0 aht;
extern Adafruit_VEML7700 veml;
extern float distance;
extern bool lowWaterLevel;

// measurment delay
unsigned long lastMeasurement = 0;
const unsigned long measureInterval = 2000; 

//  This function returns the analog data to calling function
int readSensor() {
  int soilSensorValue = analogRead(soilSensorPin);  
  int outputValue = map(soilSensorValue, 0, 1023, 255, 0); // map the 10-bit data to 8-bit data
  return outputValue;             
}

// This function returns water level as distance
float readDistance() {
  // Send imuplse
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Measure time
  long duration = pulseIn(echoPin, HIGH, 30000); // timeout 30 ms
  if (duration == 0) return -1;

  return duration * 0.0343 / 2.0;
}

// Funcrtions used by alghoritm
// This function returns temperature
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

// This function returns lux level
int checkLux(){
  float lux = veml.readLux();
  int luxLevel;
  if ( lux<10 ) luxLevel = 0;
  else luxLevel = 1;
  
  return luxLevel;
}

// This function returns humidity
float checkHumidity(){
  sensors_event_t humidityObject, tempObject;
  aht.getEvent(&humidityObject, &tempObject);
  float humidity = humidityObject.relative_humidity;

  return humidity;
}

// This function defines if the water level is sufficient
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
