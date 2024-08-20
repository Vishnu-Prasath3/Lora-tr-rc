#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <LoRa.h>

// Replace with your network credentials
const char* ssid = "D-Link_DIR-615";
const char* password = "68053222";

unsigned long previousMillis = 0;
unsigned long interval = 30000;


// it wil set the static IP address to 192, 168, 1, 184
IPAddress local_IP(192, 168, 1, 184);
//it wil set the gateway static IP address to 192, 168, 1,1
IPAddress gateway(192, 168, 1, 1);

// Following three settings are optional
IPAddress subnet(255, 255, 0, 0);
IPAddress primaryDNS(8, 8, 8, 8); 
IPAddress secondaryDNS(8, 8, 4, 4);
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
  Data to send:<br>
  <button onclick="sendData('light')">Light</button><br>
  <button onclick="sendData('fan')">Fan</button><br>
  
  <script>
    function sendData(value) {
      var xhr = new XMLHttpRequest();
      xhr.open("GET", "/get?data=" + value, true);
      xhr.onload = function() {
        if (xhr.status == 200) {
          console.log("Data sent: " + value);
          
        }
      };
      xhr.onerror = function() {
        console.error("Request failed");
      };
      xhr.send();
    }
  </script>
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

  if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS)) {
Serial.println("STA Failed to configure");
}
  // Initialize WiFi
// Connect to Wi-Fi network with SSID and password
Serial.print("Connecting to ");
Serial.println(ssid);
WiFi.begin(ssid, password);
while (WiFi.status() != WL_CONNECTED) {
delay(500);
Serial.print(".");
}
// Print local IP address and start web server
Serial.println("");
Serial.println("WiFi connected.");
Serial.println("IP address: ");
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
    } 
  });

  // Start server
  server.begin();
}

void loop() {
  // Nothing here, handled by async server
setup();
 
unsigned long currentMillis = millis();
//// if WiFi is down, try reconnecting
if ((WiFi.status() != WL_CONNECTED) && (currentMillis - previousMillis >=interval)) {
  Serial.print(millis());
  Serial.println("Reconnecting to WiFi...");
  WiFi.disconnect();
  WiFi.reconnect();
  previousMillis = currentMillis;
}
}
