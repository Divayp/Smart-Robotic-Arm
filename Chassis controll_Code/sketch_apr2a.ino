#include <WiFi.h>
#include <ESPAsyncWebServer.h>

// Motor Pins
#define EN_A 32
#define EN_B 14
#define IN1 27
#define IN2 26
#define IN3 25
#define IN4 33

// Wi-Fi Credentials
const char* ssid = "Abc";
const char* password = "9973591772";

// Create Web Server
AsyncWebServer server(80);

// HTML, CSS, and JavaScript for Mobile Control
const char webpage[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <title>ESP32 Motor Contrl</title>
  <style>
    body {
      font-family: Arial, sans-serif;
      text-align: center;
      background-color: #f3f3f3;
    }
    h1 {
      color: #333;
    }
    .circle-control {
      position: relative;
      width: 200px;
      height: 200px;
      margin: 0 auto;
    }
    button {
      position: absolute;
      width: 70px;
      height: 70px;
      border-radius: 50%;
      border: none;
      background-color: #007BFF;
      color: white;
      font-size: 16px;
      cursor: pointer;
    }
    button:hover {
      background-color: #0056b3;
    }
    #up {
      top: 0;
      left: 65px;
    }
    #down {
      bottom: 0;
      left: 65px;
    }
    #left {
      top: 65px;
      left: 0;
    }
    #right {
      top: 65px;
      right: 0;
    }
    #stop {
      top: 65px;
      left: 65px;
      background-color: #FF0000;
    }
    #stop:hover {
      background-color: #CC0000;
    }
  </style>
</head>
<body>
  <h1>ESP32 Motor Control</h1>
  <div class="circle-control">
    <button id="up" onclick="sendCommand('forward')">Up</button>
    <button id="down" onclick="sendCommand('backward')">Down</button>
    <button id="left" onclick="sendCommand('left')">Left</button>
    <button id="right" onclick="sendCommand('right')">Right</button>
    <button id="stop" onclick="sendCommand('stop')">Stop</button>
  </div>
  <script>
    function sendCommand(command) {
      fetch(`/${command}`)
        .then(response => console.log(`Command ${command} sent`))
        .catch(error => console.error('Error:', error));
    }
  </script>
</body>
</html>
)rawliteral";

void setup() {
  // Initialize Serial Communication for debugging
  Serial.begin(115200);
  Serial.println("ESP32 Motor Control is starting...");

  // Initialize motor pins
  pinMode(EN_A, OUTPUT);
  pinMode(EN_B, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  // Enable motors
  digitalWrite(EN_A, HIGH);
  digitalWrite(EN_B, HIGH);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  Serial.print("Access the web server at: ");
  Serial.println(WiFi.localIP());

  // Serve the HTML page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", webpage);
  });

  // Motor control routes
  server.on("/forward", HTTP_GET, [](AsyncWebServerRequest *request){
    moveForward();
    request->send(200, "text/plain", "Moving Forward");
  });

  server.on("/backward", HTTP_GET, [](AsyncWebServerRequest *request){
    moveBackward();
    request->send(200, "text/plain", "Moving Backward");
  });

  server.on("/left", HTTP_GET, [](AsyncWebServerRequest *request){
    turnLeft();
    request->send(200, "text/plain", "Turning Left");
  });

  server.on("/right", HTTP_GET, [](AsyncWebServerRequest *request){
    turnRight();
    request->send(200, "text/plain", "Turning Right");
  });

  server.on("/stop", HTTP_GET, [](AsyncWebServerRequest *request){
    stopMotors();
    request->send(200, "text/plain", "Stopping");
  });

  server.begin();
}

void moveForward() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  Serial.println("Moving Forward");
}

void moveBackward() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  Serial.println("Moving Backward");
}

void turnLeft() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  Serial.println("Turning Left");
}

void turnRight() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  Serial.println("Turning Right");
}

void stopMotors() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  Serial.println("Motors Stopped");
}

void loop() {
  // Nothing here, everything handled by the server
}