//*Main Color Detection Part By Using TCS230/TCS3200*
#include <ESP32Servo.h>

#define S0 4
#define S1 5
#define S2 18
#define S3 19
#define OUT 21
#define SERVO_PIN 13  // Pin connected to the servo signal

Servo myServo;
String lastDetectedColor = "";

void setup() {
  Serial.begin(115200);
  
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  pinMode(OUT, INPUT);

  digitalWrite(S0, HIGH);
  digitalWrite(S1, LOW);

  myServo.attach(SERVO_PIN);  // Attach servo to the pin
  myServo.write(0);  // Start at 0 degrees
  Serial.println("Servo setup complete");
}

int getColorFrequency(int s2State, int s3State) {
  digitalWrite(S2, s2State);
  digitalWrite(S3, s3State);
  delay(100);
  return pulseIn(OUT, LOW);
}

void loop() {
  int red = getColorFrequency(LOW, LOW);    
  int green = getColorFrequency(HIGH, HIGH); 
  int blue = getColorFrequency(LOW, HIGH);   

  String detectedColor = "";

  if (red < green && red < blue) {
    detectedColor = "RED";
  } 
  else if (green < red && green < blue) {
    detectedColor = "GREEN";
  } 
  else if (blue < red && blue < green) {
    detectedColor = "BLUE";
  } 
  else {
    detectedColor = "UNKNOWN";
  }

  // Move servo only if color changes
  if (detectedColor != lastDetectedColor) {
    lastDetectedColor = detectedColor;
    Serial.print("Detected: "); Serial.println(detectedColor);

    if (detectedColor == "RED") {
      myServo.write(60);  // Rotate to 60° for Red
      Serial.println("Servo moved to 60° (RED)");
    } else if (detectedColor == "GREEN") {
      myServo.write(120); // Rotate to 120° for Green
      Serial.println("Servo moved to 120° (GREEN)");
    } else if (detectedColor == "BLUE") {
      myServo.write(180); // Rotate to 180° for Blue
      Serial.println("Servo moved to 180° (BLUE)");
    }
  }

  delay(4000); // Wait 4 seconds before next detection
}