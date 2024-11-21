#include <Adafruit_BMP085.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <stdio.h>
#include "driver/rtc_io.h"

// Server variables
const char* ssid = "Joshua's iPhone";
const char* password = "buymyselfflowers";

const char* laptopAt = "http://192.168.14.145:3237";  //change to your Laptop's IP

const int period = 60;

// for BMP180
Adafruit_BMP085 bmp;

// for anemometer
#define WIND_PIN 13     // connect anememometer to this pin
int count = 0;

// for DHT22
#define DHTPIN 5       // Digital pin connected to the DHT sensor
#define DHTTYPE DHT11  // DHT 11
DHT_Unified dht(DHTPIN, DHTTYPE);

char* url;
float windSpeed;
float bmpTemp;
int bmpPressure;
float dhtTemp;
float dhtHumidity;

void countWind()
{
  if (digitalRead(WIND_PIN) == LOW)
    count++;
}

void setup() {
  Serial.begin(115200);  //Initialize serial port

  // enable interrupt for anemometer
  pinMode(WIND_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(WIND_PIN), countWind, FALLING);

  if (!bmp.begin()) {
    Serial.println("Could not find a valid BMP085 sensor, check wiring!");
  }

  dht.begin();
  sensor_t sensor;
  dht.temperature().getSensor(&sensor);
  Serial.println(F("------------------------------------"));
  Serial.println(F("Temperature Sensor"));
  Serial.print(F("Sensor Type: "));
  Serial.println(sensor.name);
  Serial.print(F("Driver Ver:  "));
  Serial.println(sensor.version);
  Serial.print(F("Unique ID:   "));
  Serial.println(sensor.sensor_id);
  Serial.print(F("Max Value:   "));
  Serial.print(sensor.max_value);
  Serial.println(F("°C"));
  Serial.print(F("Min Value:   "));
  Serial.print(sensor.min_value);
  Serial.println(F("°C"));
  Serial.print(F("Resolution:  "));
  Serial.print(sensor.resolution);
  Serial.println(F("°C"));
  Serial.println(F("------------------------------------"));
  dht.humidity().getSensor(&sensor);
  Serial.println(F("Humidity Sensor"));
  Serial.print(F("Sensor Type: "));
  Serial.println(sensor.name);
  Serial.print(F("Driver Ver:  "));
  Serial.println(sensor.version);
  Serial.print(F("Unique ID:   "));
  Serial.println(sensor.sensor_id);
  Serial.print(F("Max Value:   "));
  Serial.print(sensor.max_value);
  Serial.println(F("%"));
  Serial.print(F("Min Value:   "));
  Serial.print(sensor.min_value);
  Serial.println(F("%"));
  Serial.print(F("Resolution:  "));
  Serial.print(sensor.resolution);
  Serial.println(F("%"));
  Serial.println(F("------------------------------------"));

}

void loop() {
  send_data();
  delay(period * 1000);
}

void send_data() {

  // wind speed readings
  windSpeed = (count * 8.75) / 100 / period;
  Serial.print("Wind Speed: ");
  Serial.print(windSpeed);
  Serial.println("m/s");
  count = 0;

  // BMP180 readings
  bmpTemp = bmp.readTemperature();
  Serial.print("BMP180 temperature: ");
  Serial.print(bmpTemp);
  Serial.println(" *C");

  bmpPressure = bmp.readPressure();
  Serial.print("BMP180 pressure: ");
  Serial.print(bmpPressure);
  Serial.println(" Pa");

  // DHT11 readings
  sensors_event_t event;
  dht.temperature().getEvent(&event);
  if (isnan(event.temperature)) {
    Serial.println(F("Error reading temperature!"));
  } else {
    dhtTemp = event.temperature;
    Serial.print(F("DHT11 temperature: "));
    Serial.print(dhtTemp);
    Serial.println(F("°C"));
  }
  dht.humidity().getEvent(&event);
  if (isnan(event.relative_humidity)) {
    Serial.println(F("Error reading humidity!"));
  } else {
    dhtHumidity = event.relative_humidity;
    Serial.print(F("DHT11 humidity: "));
    Serial.print(dhtHumidity);
    Serial.println(F("%"));
  }

  // connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  
  HTTPClient http;

  // send readings to server
  url = (char*)malloc(132);
  snprintf(url, 132, "%s/data?windSpeed=%.2f&bmpTemp=%.2f&bmpPressure=%d&dhtTemp=%.2f&dhtHumidity=%.2f",
           laptopAt, windSpeed, bmpTemp, bmpPressure, dhtTemp, dhtHumidity);
  http.begin(url);
  Serial.println(url);
  int returnCode = http.GET();  //perform a HTTP GET request

  Serial.println(returnCode);

  if (returnCode > 0) {
    String payload = http.getString();
    Serial.println(payload);
  }
  http.end();
  free(url);
  WiFi.disconnect();
  Serial.println();
}
