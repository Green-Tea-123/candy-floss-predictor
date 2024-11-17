#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <ESP32Servo.h>

Servo clothesServo;
Servo windowServo;

int clothesPos = 0;
int clothesServoPin = 17;
int windowPos = 60;
int windowServoPin = 15;
int windowOpen = true;

const char* ssid = "[REDACTED]";
const char* password = "[REDACTED]";

WebServer server(80);

void handleRaining(); // Function prototype for handleRaining
void handleNotFound();
void closeWindow();
void openWindow();

void setup(void) {
  Serial.begin(115200);
  // servo code
  ESP32PWM::allocateTimer(0);
	ESP32PWM::allocateTimer(1);
	ESP32PWM::allocateTimer(2);
	ESP32PWM::allocateTimer(3);
	clothesServo.setPeriodHertz(50);
  clothesServo.write(clothesPos);
	clothesServo.attach(clothesServoPin, 500, 2500);
  windowServo.setPeriodHertz(50);
  windowServo.write(windowPos);
  windowServo.attach(windowServoPin, 500, 2500);
  // wifi code
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp32")) {
    Serial.println("MDNS responder started");
  }

  server.on("/raining", HTTP_POST, handleRaining);

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

void handleRaining() {
  if (server.hasArg("plain")) {
    String data = server.arg("plain"); // either 1 or 0
    Serial.println(data);
    if (data.equals("1")) {
      Serial.println("is raining");
      closeWindow();
    } else {
      Serial.println("is not raining");
      openWindow();
    }
    // handle data here
    server.send(200, "text/plain", "Data received");
  } else {
    server.send(400, "text/plain", "No data received");
    Serial.println("Data not received");
  }
}

void loop() {
  server.handleClient(); // This allows the server to process incoming requests
}

void closeWindow() {
  if (windowOpen) {
    // move clothesline in first
    for (; clothesPos <= 240; clothesPos += 1) {
      clothesServo.write(clothesPos);
      delay(2);
    }
    delay(500);
    
    // close window
    for (; windowPos <= 110; windowPos += 1) {
      windowServo.write(windowPos);
      delay(10);
    }
    windowOpen = false;
  }
}

void openWindow() {
  if (!windowOpen) { 
    // open window first
    for (; windowPos >= 40; windowPos -= 1) {
      windowServo.write(windowPos);
      delay(10);
    }
    delay(500);

    // move clothesline out
    for (; clothesPos >= 0; clothesPos -= 1) {
      clothesServo.write(clothesPos);
      delay(2);
    }
    windowOpen = true;
  }
}
