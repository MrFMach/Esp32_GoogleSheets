#include <HTTPClient.h>
#include <WiFi.h>
#include "string.h"
#include <Wire.h>
#include <Adafruit_BMP085.h>

float temperatura = 0;
int pressao = 0;
float altitude = 0;

// Create object
Adafruit_BMP085 bmp;

const char* ssid     = "";     // your network SSID (name of wifi network)
const char* password = ""; // your network password

char *server = "script.google.com";  // Server URL

char *GScriptId = "AKfycbxyxUyntbTZiKqg8MPitN2ceWRkFX4EPgBz5nxzxhkNbDStbUHQZofBdBw-Gm6O0-Nv"; // script id, part of script link
                
const int httpsPort = 443;

void startBmp() {
  Serial.println("Starting BMP180...");
  if (!bmp.begin())
  {
    Serial.println("Could not find a valid BMP180 sensor, check wiring!");
    while (1)
    {
    }
  }
}

void startWifi(){
  Serial.print("Connecting to wifi ...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(750);
    Serial.print(".");
  }

  Serial.println("");
  Serial.print("WiFi connected! Rede: ");
  Serial.print(ssid);
  Serial.print(", IP: ");
  Serial.println(WiFi.localIP());
}

void ScriptCode( void * pvParameters ){
  HTTPClient http;
  String payload;

  while(1) {

    String url = String("https://script.google.com") + "/macros/s/" + GScriptId + "/exec?" + "value1=" + String(temperatura, 3) + "&value2=" + pressao + "&value3=" + String(altitude, 3);
    
    // Making a request
    http.begin(url.c_str()); //Specify the URL and certificate
    http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
    int httpCode = http.GET();

    Serial.print("Core ID: ");
    Serial.print(xPortGetCoreID());

    if (httpCode > 0) {
      payload = http.getString();
      Serial.print(" - Payload: ");
      Serial.println(payload);
    }
    else {
      Serial.println(" (!) Error on HTTP request");
    }
    http.end();

    vTaskDelay(600000 / portTICK_PERIOD_MS);
  }
}

void setup() {
  pinMode(2, OUTPUT);
  Serial.begin(115200); 
  startBmp();
  startWifi();
           
  xTaskCreatePinnedToCore(ScriptCode,"Script",10000,NULL,5,NULL,0);
}

void loop() {

    // Disable watch dog time
  disableCore0WDT();
  disableCore1WDT();

  Serial.print("Core ID: ");
  Serial.print(xPortGetCoreID());
  Serial.print(" - Leitura do Sensor: ");

  // read sensor
  temperatura = bmp.readTemperature();
  pressao = bmp.readPressure();
  altitude = bmp.readAltitude();

  Serial.print(temperatura);
  Serial.print(" °C");
  Serial.print("  |  ");
  Serial.print(pressao);
  Serial.print(" Pa");
  Serial.print("  |  ");
  Serial.print(altitude);
  Serial.println(" m");

  digitalWrite(2, HIGH);  // blink built in led
  vTaskDelay(200 / portTICK_PERIOD_MS);
  digitalWrite(2, LOW);
  delay(800);
 }