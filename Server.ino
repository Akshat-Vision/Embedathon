#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <AsyncWebSocket.h>

const char* ssid = "Xiaomi 11T Pro";
const char* password = "akshatsh23";

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

String passwords[] = {
  "18141312131254144313133",
  "1711131213131415111313121312131",
  "16131211141314141312131213121",
  "16131225541413124313133",
  "1652111413141452111413161",
  "1613121213131414131212131312131",
  "521312131213141413121313343"
};

String combinedPasswords = "";

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  Serial.print("ESP32 IP Address: ");
  Serial.println(WiFi.localIP());

  for (int i = 0; i < sizeof(passwords) / sizeof(passwords[0]); i++) {
    combinedPasswords += passwords[i];
    if (i < sizeof(passwords) / sizeof(passwords[0]) - 1) {
      combinedPasswords += ", ";
    }
  }

  ws.onEvent([](AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
    if (type == WS_EVT_CONNECT) {
      Serial.println("WebSocket client connected");
      client->text(combinedPasswords);
      Serial.println("Sent all passwords: " + combinedPasswords);
    } else if (type == WS_EVT_DISCONNECT) {
      Serial.println("WebSocket client disconnected");
    } else if (type == WS_EVT_DATA) {
      Serial.println("WebSocket data received");
      handleWebSocketMessage(arg, data, len);
    } else {
      Serial.println("Unexpected WebSocket event");
    }
  });

  server.addHandler(&ws);

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    String html = "<!DOCTYPE html><html><body>";
    html += "<h1>ESP32 WebSocket Server</h1>";
    html += "<p>WebSocket Server running on ESP32. Send a message and it will be echoed back.</p>";
    html += "<input type='text' id='messageInput' placeholder='Type a message...'>";
    html += "<button onclick='sendMessage()'>Send Message</button>";
    html += "<script>";
    html += "var socket = new WebSocket('ws://192.168.29.4/ws');";
    html += "socket.onmessage = function(event) { alert('Received: ' + event.data); };";
    html += "socket.onopen = function(event) { console.log('WebSocket connection established'); };";
    html += "socket.onerror = function(error) { console.log('WebSocket error:', error); };";
    html += "function sendMessage() {";
    html += "  var message = document.getElementById('messageInput').value;";
    html += "  socket.send(message);";
    html += "  console.log('Sent: ' + message);";
    html += "}"; 
    html += "</script>";
    html += "</body></html>";
    request->send(200, "text/html", html);
  });

  server.begin();
}

void loop() {
  ws.cleanupClients();
}

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
  String message = String((char*)data);
  Serial.println("Received WebSocket message: " + message);
  ws.textAll("Echo: " + message);
}
