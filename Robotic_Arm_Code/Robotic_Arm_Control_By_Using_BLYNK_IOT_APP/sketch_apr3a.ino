// Blynk IoT Configuration (REPLACE THESE WITH YOUR CREDENTIALS)
#define BLYNK_TEMPLATE_ID "TMPL6ZQmi74_2"
#define BLYNK_TEMPLATE_NAME "Nodemcu"
#define BLYNK_AUTH_TOKEN "8gU9uTR-mKkuAiIUmEJBF-nbfKqqKl9r"     // From Blynk Device settings

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <ESP32Servo.h>

// Servo Configuration (5 servos)
#define NUM_SERVOS 5
Servo servos[NUM_SERVOS];
const int servoPins[NUM_SERVOS] = {27, 26, 25, 33, 32}; // Base, Shoulder, Elbow, Gripper, Wrist

// WiFi Credentials
char ssid[] = "Abc";
char pass[] = "9973591772";

// Record/Playback System
struct RecordedStep {
  uint8_t servoIndex;  // 0-4
  uint8_t angle;       // 0-180
  unsigned long delayMs;
};
std::vector<RecordedStep> recordedSteps;

bool isRecording = false;
bool isPlaying = false;
unsigned long lastRecordTime = 0;
unsigned long playbackStartTime = 0;
size_t currentStep = 0;

// Blynk Virtual Pins
#define V_BASE      V0
#define V_SHOULDER  V1
#define V_ELBOW     V2
#define V_GRIPPER   V3
#define V_WRIST     V4
#define V_RECORD    V5
#define V_PLAY      V6
#define V_STATUS    V7

void setup() {
  Serial.begin(115200);
  
  // Initialize servos with error checking
  for(int i = 0; i < NUM_SERVOS; i++) {
    if(servos[i].attach(servoPins[i])) {
      servos[i].write(90); // Initial position
      Serial.printf("Servo %d attached to pin %d\n", i, servoPins[i]);
    } else {
      Serial.printf("Failed to attach servo %d to pin %d\n", i, servoPins[i]);
    }
  }
  
  // Initialize Blynk
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  Serial.println("Connecting to Blynk...");
}

void loop() {
  Blynk.run();
  handlePlayback();
}

void safeServoWrite(uint8_t index, uint8_t angle) {
  if(index < NUM_SERVOS) {
    servos[index].write(angle);
    
    // Record movement if enabled
    if(isRecording) {
      unsigned long now = millis();
      recordedSteps.push_back({index, angle, now - lastRecordTime});
      lastRecordTime = now;
      Serial.printf("Recorded: Servo%d=%d, Delay=%lums\n", index, angle, now - lastRecordTime);
    }
  }
}

void handlePlayback() {
  if(!isPlaying || recordedSteps.empty()) return;

  static bool initialPositioning = true;
  
  if(initialPositioning) {
    // Move to initial positions
    for(int i = 0; i < NUM_SERVOS; i++) {
      servos[i].write(recordedSteps[i].angle);
    }
    delay(2000);
    initialPositioning = false;
    playbackStartTime = millis();
    return;
  }

  if(currentStep < recordedSteps.size()) {
    if(millis() - playbackStartTime >= recordedSteps[currentStep].delayMs) {
      safeServoWrite(recordedSteps[currentStep].servoIndex, 
                    recordedSteps[currentStep].angle);
      playbackStartTime = millis();
      currentStep++;
    }
  } else {
    stopPlayback();
  }
}

void stopPlayback() {
  isPlaying = false;
  currentStep = 0;
  Blynk.virtualWrite(V_PLAY, 0);
  Blynk.virtualWrite(V_STATUS, "Playback Complete");
  Serial.println("Playback finished");
}

// Blynk Input Handlers
BLYNK_WRITE(V_BASE)     { safeServoWrite(0, param.asInt()); }
BLYNK_WRITE(V_SHOULDER) { safeServoWrite(1, param.asInt()); }
BLYNK_WRITE(V_ELBOW)    { safeServoWrite(2, param.asInt()); }
BLYNK_WRITE(V_GRIPPER)  { safeServoWrite(3, param.asInt()); }
BLYNK_WRITE(V_WRIST)    { safeServoWrite(4, param.asInt()); }

BLYNK_WRITE(V_RECORD) {
  isRecording = param.asInt();
  if(isRecording) {
    recordedSteps.clear();
    lastRecordTime = millis();
    Blynk.virtualWrite(V_STATUS, "Recording...");
    Serial.println("Recording started");
  } else {
    Blynk.virtualWrite(V_STATUS, "Recording Saved");
    Serial.println("Recording saved");
  }
}

BLYNK_WRITE(V_PLAY) {
  isPlaying = param.asInt();
  if(isPlaying) {
    if(!recordedSteps.empty()) {
      currentStep = 0;
      Blynk.virtualWrite(V_STATUS, "Playing...");
      Serial.println("Playback started");
    } else {
      Blynk.virtualWrite(V_STATUS, "No Recording!");
      Blynk.virtualWrite(V_PLAY, 0);
      Serial.println("Playback attempted with empty recording");
    }
  }
}

BLYNK_CONNECTED() {
  // Sync servo positions on connection
  for(int i = 0; i < NUM_SERVOS; i++) {
    Blynk.virtualWrite(i, servos[i].read());
  }
  Blynk.virtualWrite(V_STATUS, "Connected");
  Serial.println("Blynk connected");
}