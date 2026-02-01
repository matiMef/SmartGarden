#include "Sensors.h"

extern Adafruit_AHTX0 aht;
extern Adafruit_VEML7700 veml;
extern float distance;
extern bool lowWaterLevel;
SensorData globalSensors;

// measurment delay
unsigned long lastMeasurement = 0;
const unsigned long measureInterval = 2000;

bool notConnected = false;

void readAllSensors() {
  const int stepSize = 8;

  float soilSum = 0;
  float luxSum = 0;
  float humSum = 0;
  float tempSum = 0;

  sensors_event_t humidity, temp;

  bool ahtError = false;
  bool vemlError = false;
  bool soilError = false;

  if (!isAHTPresent()) {
    ahtError = true;
    tempSum = 25 * stepSize;
    humSum = 50 * stepSize;
    Serial.println("AHT ERROR");
    I2C_Recover();
    aht.begin();
    Serial.println("AHT RECONFIGURED");
  } else {
    ahtError = false;
  }

  if (!isVEMLPresent()) {
    vemlError = true;
    luxSum = 10 * stepSize;
    Serial.println("VEML ERROR");
    I2C_Recover();
  } else {
    vemlError = false;
    if (!veml.enabled()) {
      Serial.println("VEML RECONFIGURED");
      veml.enable(1);
      veml.begin();
      veml.setGain(VEML7700_GAIN_1);
      veml.setIntegrationTime(VEML7700_IT_100MS);
    }
  }

  for (int i = 0; i < stepSize; i++) {
    soilSum += readSensor();
    if (!vemlError) {
      luxSum += veml.readLux();
    }
    delay(120);
    if (!ahtError) {
      aht.getEvent(&humidity, &temp);
      humSum += humidity.relative_humidity;
      tempSum += temp.temperature;
    }
  }
  calculateDistance();


  if (soilSum < 0 || soilSum > 255 * stepSize) {
    soilError = true;
    Serial.println("SOIL ERROR");
    soilSum = 2550;
  } else {
    soilError = false;
  }

  if (soilSum < 15 * stepSize) {
    notConnected = true;
    Serial.println("SOIL SENSOR NOT CONNECTED");
    soilSum = 255 * stepSize;
  }

  if (humSum <= 0) {
    humSum = 50 * stepSize;
    Serial.println("HUM ERROR");
  }

  if (luxSum < 0) {
    luxSum = 10 * stepSize;
    Serial.println("LUX ERROR");
  }

  if (distance <= 1) {
    distance = 13.00;
    Serial.println("DISTANCE ERROR");
  }

  globalSensors.soilHumidity = round(soilSum / stepSize);
  Serial.println("--------------------------------");
  Serial.print("Soil Humidity: ");
  Serial.println(globalSensors.soilHumidity);
  globalSensors.lux = luxSum / stepSize;
  Serial.print("Lux:");
  Serial.println(globalSensors.lux);
  globalSensors.airHumidity = humSum / stepSize;
  Serial.print("Air Himidity: ");
  Serial.println(globalSensors.airHumidity);
  globalSensors.temperature = tempSum / stepSize;
  Serial.print("Temperature: ");
  Serial.println(globalSensors.temperature);
  globalSensors.waterDistance = distance;
  Serial.print("Water distance: ");
  Serial.println(globalSensors.waterDistance);
  Serial.println("--------------------------------");

  // globalSensors.soilHumidity = 75;
  // Serial.println("--------------------------------");
  // Serial.print("Soil Humidity: ");
  // Serial.println(globalSensors.soilHumidity);
  // globalSensors.lux = 5;
  // Serial.print("Lux:");
  // Serial.println(globalSensors.lux);
  // globalSensors.airHumidity = 50;
  // Serial.print("Air Himidity: ");
  // Serial.println(globalSensors.airHumidity);
  // globalSensors.temperature = 32;
  // Serial.print("Temperature: ");
  // Serial.println(globalSensors.temperature);
  // globalSensors.waterDistance = distance;
  // Serial.print("Water distance: ");
  // Serial.println(globalSensors.waterDistance);
  // Serial.println("--------------------------------");
}

//  This function returns the analog data to calling function
int readSensor() {
  int soilSensorValue = analogRead(soilSensorPin);
  int outputValue = map(soilSensorValue, 0, 1023, 255, 0);  // map the 10-bit data to 8-bit data
  return outputValue;
}

bool isAHTPresent() {
  Wire.beginTransmission(0x38);
  return (Wire.endTransmission() == 0);
}

bool isVEMLPresent() {
  Wire.beginTransmission(0x10);
  return (Wire.endTransmission() == 0);
}

void I2C_Recover() {
  pinMode(A4, INPUT_PULLUP);  // SDA
  pinMode(A5, INPUT_PULLUP);  // SCL

  // 9 clock pulses
  for (int i = 0; i < 9; i++) {
    pinMode(A5, OUTPUT);
    digitalWrite(A5, LOW);
    delayMicroseconds(5);
    digitalWrite(A5, HIGH);
    delayMicroseconds(5);
  }

  // STOP condition
  pinMode(A4, OUTPUT);
  digitalWrite(A4, LOW);
  delayMicroseconds(5);
  digitalWrite(A4, HIGH);

  // Restore I2C
  Wire.begin();
  Serial.println("I2C RECOVERED");
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
  long duration = pulseIn(echoPin, HIGH, 30000);  // timeout 30 ms
  if (duration == 0) return -1;

  return duration * 0.0343 / 2.0;
}

// Funcrtions used by alghoritm
// This function returns temperature
int checkTemperature() {
  float temp = globalSensors.temperature;
  int tempLevel;  // 0=zimno, 1=optymalnie, 2=gorąco

  if (temp < 15) tempLevel = 0;
  else if (temp < 26) tempLevel = 1;
  else if (temp < 31) tempLevel = 2;
  else tempLevel = 3;

  return tempLevel;
}

// This function returns lux level
int checkLux() {
  float lux = globalSensors.lux;
  int luxLevel;
  if (lux < 10) luxLevel = 0;
  else luxLevel = 1;

  return luxLevel;
}

// This function defines if the water level is sufficient
void calculateDistance() {
  if (millis() - lastMeasurement >= measureInterval) {
    lastMeasurement = millis();
    distance = readDistance();
    // Serial.println(lowWaterLevel);
    if (distance > 10) {
      lowWaterLevel = true;
    } else {
      lowWaterLevel = false;
    }
  }
}
