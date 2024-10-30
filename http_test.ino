#include <WiFi.h>
#include <HTTPClient.h>

// Replace with your network credentials
const char* ssid = "ESPtest";
const char* password = "244466666";

void setup() {
    Serial.begin(115200);
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to WiFi...");
    }

    Serial.println("Connected to WiFi");
    delay(2000);
}

void loop() {
    // Simulating sensor data
    float sensorValue = 2314.202; //analogRead(34);  // Replace with your sensor pin

    // Create JSON payload
    String jsonData = "{\"sensor_value\":" + String(sensorValue) + "}";

    // Send data to the server
    if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;
        http.begin("http://172.20.10.12:5001/sensor");  // Replace with your MacBook's local IP

        http.addHeader("Content-Type", "application/json");
        int httpResponseCode = http.POST(jsonData);

        if (httpResponseCode > 0) {
            String response = http.getString();
            Serial.println(httpResponseCode);
            Serial.println(response);
        } else {
            Serial.print("Error on sending POST: ");
            Serial.println(httpResponseCode);
        }

        http.end();
    }

    delay(5000);  // Send data every 5 seconds
}