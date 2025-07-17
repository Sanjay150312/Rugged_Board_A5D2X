#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "sanjay";
const char* password = "san123455";

WebServer server(80);
String distanceValue = "Waiting...";
bool requestDistance = false;

void handleRoot() {
  String html = "<!DOCTYPE html><html><head><meta charset='UTF-8'>";
  html += "<title>ESP32 Sensor Panel</title>";
  html += "<style>body{font-family:Arial;text-align:center;}button{padding:10px 20px;margin:10px;font-size:16px;}</style>";
  html += "</head><body><h2>Read Ultrasonic Sensor from Rugged Board</h2>";
  html += "<p>Distance: <strong>" + distanceValue + "</strong></p>";
  html += "<form action='/read' method='GET'>";
  html += "<button name='cmd' value='DIST'>Get Distance</button>";
  html += "</form></body></html>";
  server.send(200, "text/html", html);
}

void handleRead() {
  if (server.hasArg("cmd") && server.arg("cmd") == "DIST") {
    requestDistance = true;
    Serial2.println("GETDIST");  // Request to rugged board
    Serial.println("Command sent: GETDIST");
  }
  server.sendHeader("Location", "/");
  server.send(303);
}

void setup() {
  Serial.begin(115200);  // Debug output
  Serial2.begin(115200, SERIAL_8N1, 16, 17);  // UART2: RX=16, TX=17

  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nConnected! IP: " + WiFi.localIP().toString());

  server.on("/", handleRoot);
  server.on("/read", handleRead);
  server.begin();
}

void loop() {
  server.handleClient();

  if (requestDistance && Serial2.available()) {
    String incoming = Serial2.readStringUntil('\n');
    if (incoming.startsWith("DIST=")) {
      distanceValue = incoming.substring(5) + " cm";
      Serial.println("Received: " + distanceValue);
      requestDistance = false;
    }
  }
}
