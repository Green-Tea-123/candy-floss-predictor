#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>

const char* ssid = "JB's phone";
const char* password = "eeffoc8257";

WebServer server(80);

void handleRaining(); // Function prototype for handleRaining
void handleNotFound();

void setup(void) {
  Serial.begin(115200);
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
    } else {
      Serial.println("is not raining");
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
