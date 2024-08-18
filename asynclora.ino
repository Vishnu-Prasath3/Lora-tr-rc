#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <LoRa.h>

// Replace with your network credentials
const char* ssid = "D-Link_DIR-615";
const char* password = "68053222";

// LoRa Settings
#define SS 5
#define RST 14
#define DI0 4
#define BAND 433E6

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

String dataToSend = "";

// HTML form to input data
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html><body>
<form action="/get">
  Data to send:<br>
  <button id="bt1">Light</button>
  <button id="bt2">Fan</button>
   <input type="text" name="data">
  <input type="submit" value="Send">
</form>
</body></html>)rawliteral";

void setup() {
  // Start serial
  Serial.begin(115200);

  // Initialize LoRa
  LoRa.setPins(SS, RST, DI0);
  if (!LoRa.begin(BAND)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
  
  // Initialize WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  Serial.println(WiFi.localIP());
  // Serve the HTML form
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html);
  });

  // Handle form submit
  server.on("/get", HTTP_GET, [](AsyncWebServerRequest *request){
    if (request->hasParam("data")) {
      dataToSend = request->getParam("data")->value();
      Serial.println("Data received: " + dataToSend);

      // Send data over LoRa
      LoRa.beginPacket();
      LoRa.print(dataToSend);
      LoRa.endPacket();

      // request->send(200, "text/plain", "Data sent over LoRa: " + dataToSend);
    } else {
      request->send(200, "text/plain", "No data received");
    }
  });

  // Start server
  server.begin();
}

void loop() {
  // Nothing here, handled by async server
}
