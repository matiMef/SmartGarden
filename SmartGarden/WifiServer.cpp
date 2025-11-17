#include "WifiServer.h"
#include <EEPROM.h>

extern WiFiServer server;
extern Adafruit_AHTX0 aht;
extern Adafruit_VEML7700 veml;
extern bool hold;
extern bool wateringActive;
extern bool lowWaterLevel;
extern int profile;
extern bool configuration;

extern const char* apSsid;
extern const char* apPassword;

char tempApSsid[] = "Arduino";
char tempApPassword[] = "SmartGarden";

String apIp = "";

int readSensor();
float readDistance();
void shortWatering();
void midWatering();
void longWatering();

// This function communicates with backend
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
        json += "\"distance\":" + String(distance) + ",";
        json += "\"profile\":" + String(profile);
        json += "}";

        client.println("HTTP/1.1 200 OK");
        client.println("Content-Type: application/json");
        client.println("Connection: close");
        client.println();
        client.println(json);
      }

        if (req.indexOf("POST /command") != -1) 
          {
            while (client.available()) 
            {
              String body = client.readStringUntil('\n');
              if (body.length() == 0) break; 
            }
            if (!lowWaterLevel && !wateringActive)
            {
              String payload = client.readString();

              if (payload.indexOf("start_short") != -1){
                hold = false;
                shortWatering();
              }
              else if(payload.indexOf("start_mid") != -1){
                hold = false;
                midWatering();
              }
              else if(payload.indexOf("start_long") != -1){
                hold = false;
                longWatering();
              }
                client.println("HTTP/1.1 200 OK");
                client.println("Content-Type: text/plain");
                client.println("Connection: close");
                client.println(); 
                client.println("OK");
            }
            else if(lowWaterLevel){
              client.println("HTTP/1.1 200 OK");
              client.println("Content-Type: text/plain");
              client.println("Connection: close");
              client.println();
              client.println("LOW_WATER_LEVEL");
            }
            else{
              client.println("HTTP/1.1 200 OK");
              client.println("Content-Type: text/plain");
              client.println("Connection: close");
              client.println();
              client.println("WATERING_ACTIVE");
            }
          }
      
    }
    delay(1);
    client.stop();
  }
}

void writeStringToEEPROM(int addrOffset, const String &strToWrite)
{
  byte len = strToWrite.length();
  EEPROM.write(addrOffset, len);
  for (int i = 0; i < len; i++)
  {
    EEPROM.write(addrOffset + 1 + i, strToWrite[i]);
  }
}

String readStringFromEEPROM(int addrOffset)
{
  int newStrLen = EEPROM.read(addrOffset);
  char data[newStrLen];
  Serial.println(newStrLen);
  for (int i = 0; i < newStrLen; i++)
  {
    data[i] = EEPROM.read(addrOffset + 1 + i);
  }
  data[newStrLen] = '\0';
  return String(data);
}

int readStringLenght(int addrOffset)
{
  int strLen = EEPROM.read(addrOffset);
  return strLen;
}

void createAccessPoint(){
  IPAddress confIp(192, 168, 5, 1);
  WiFi.config(confIp);
  if (WiFi.beginAP(tempApSsid, tempApPassword) != WL_AP_LISTENING){
    WiFi.beginAP(tempApSsid, tempApPassword);
    delay(100);
  }
  apIp = WiFi.localIP().toString();
  // Serial.println(apIp);

  WiFiClient client = server.available();
   if (client) {
    Serial.println("Client connected");
    String req = client.readStringUntil('\r');
    Serial.println(req);
    client.flush();

    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/html");
    client.println("Connection: close");
    client.println();
    client.println("<!DOCTYPE html><html><body>");
    client.println("<h1>Smart Garden WiFi Manager</h1>");
    client.println("<form action='/connect' method='GET'>");
    client.println("SSID: <input name='ssid'><br>");
    client.println("Password: <input name='pass' type='password'><br>");
    client.println("<input type='submit' value='Connect'>");
    client.println("</form>");
    client.println("</body></html>");

    if (req.indexOf("GET /connect?ssid=") != -1) {
      int ssidStart = req.indexOf("ssid=") + 5;
      int passStart = req.indexOf("pass=") + 5;

      String newSsid = req.substring(ssidStart, req.indexOf('&', ssidStart));
      String newPass = req.substring(passStart, req.indexOf(' ', passStart));
      
      Serial.println("SSID: " + newSsid);
      Serial.println("Password: " + newPass);
      int newOffset = newSsid.length()+2;
      writeStringToEEPROM(1, newSsid);
      writeStringToEEPROM(newOffset, newPass);
      configuration = false;
      WiFi.disconnect();
      delay(100);
    }
      client.stop();
      delay(1);
      Serial.println("Client disconnected");
    }
}