#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

/* WiFi Settings */
const char* ssid = "TP-Link_8E98";
const char* password = "86427920";

/* Fixed Speed Value (0 to 1023) */
const int FIXED_SPEED = 800; 

/* Motor Driver Pins */
const int motor1Pin1 = D2; // IN1
const int motor1Pin2 = D3; // IN2
const int motor2Pin1 = D4; // IN3
const int motor2Pin2 = D5; // IN4
const int enaPin = D6;     // Speed Motor 1
const int enbPin = D7;     // Speed Motor 2
const int relayPin = D1;   // LED Relay

bool isAutoMode = false;   // Variable to track mode

ESP8266WebServer server(80);

void setup() {
  Serial.begin(115200);

  pinMode(motor1Pin1, OUTPUT);
  pinMode(motor1Pin2, OUTPUT);
  pinMode(motor2Pin1, OUTPUT);
  pinMode(motor2Pin2, OUTPUT);
  pinMode(enaPin, OUTPUT);
  pinMode(enbPin, OUTPUT);
  pinMode(relayPin, OUTPUT);
    //digitalWrite(relayPin, HIGH);

  stopMotors();
  digitalWrite(relayPin, HIGH); // LEDs OFF

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) { delay(500); }

  /* Mode Selection Routes */
  server.on("/m", []() { 
    isAutoMode = false; 
    stopMotors(); 
    server.send(200, "text/plain", "Manual Mode Engaged"); 
  });
  
  server.on("/a", []() { 
    isAutoMode = true; 
    server.send(200, "text/plain", "Auto Mode Engaged"); 
  });

  /* Movement Routes */
  server.on("/f", moveForward);
  server.on("/b", moveBackward);
  server.on("/l", turnLeft);
  server.on("/r", turnRight);
  server.on("/s", stopMotors);

  /* LED Routes */
  server.on("/lon", []() { Serial.println("LEDON..."); digitalWrite(relayPin, LOW); server.send(200, "text/plain", "LED ON"); });
  server.on("/loff", []() { Serial.println("LEDOFF..."); digitalWrite(relayPin, HIGH); server.send(200, "text/plain", "LED OFF"); });

  server.begin();
}

void loop() {
  server.handleClient();

  if (isAutoMode) {
    // Add your autonomous logic here (e.g., Ultrasonic obstacle avoidance)
    // For now, it just prints status to Serial
    Serial.println("Auto mode running...");
    delay(500); 
  }
}

/* Control Functions */
void moveForward() {
  Serial.println("Forward...");
  if (isAutoMode) return; // Ignore manual commands if in Auto
  analogWrite(enaPin, FIXED_SPEED); analogWrite(enbPin, FIXED_SPEED);
  digitalWrite(motor1Pin1, LOW);  digitalWrite(motor1Pin2, HIGH);
  digitalWrite(motor2Pin1, LOW);  digitalWrite(motor2Pin2, HIGH);
  server.send(200, "text/plain", "Forward");
}

void moveBackward() {
  Serial.println("Backward...");
  if (isAutoMode) return;
  analogWrite(enaPin, FIXED_SPEED); analogWrite(enbPin, FIXED_SPEED);
  digitalWrite(motor1Pin1, HIGH); digitalWrite(motor1Pin2, LOW);
  digitalWrite(motor2Pin1, HIGH); digitalWrite(motor2Pin2, LOW);
  server.send(200, "text/plain", "Backward");
}

void turnLeft() {
  Serial.println("Left...");
  if (isAutoMode) return;
  analogWrite(enaPin, FIXED_SPEED); analogWrite(enbPin, FIXED_SPEED);
  digitalWrite(motor1Pin1, HIGH); digitalWrite(motor1Pin2, LOW);
  digitalWrite(motor2Pin1, LOW);  digitalWrite(motor2Pin2, HIGH);
  server.send(200, "text/plain", "Left");
}

void turnRight() {
  Serial.println("Right...");
  if (isAutoMode) return;
  analogWrite(enaPin, FIXED_SPEED); analogWrite(enbPin, FIXED_SPEED);
  digitalWrite(motor1Pin1, LOW);  digitalWrite(motor1Pin2, HIGH);
  digitalWrite(motor2Pin1, HIGH); digitalWrite(motor2Pin2, LOW);
  server.send(200, "text/plain", "Right");
}

void stopMotors() {
  Serial.println("Stop...");
  analogWrite(enaPin, 0); analogWrite(enbPin, 0);
  digitalWrite(motor1Pin1, LOW); digitalWrite(motor1Pin2, LOW);
  digitalWrite(motor2Pin1, LOW); digitalWrite(motor2Pin2, LOW);
  server.send(200, "text/plain", "Stopped");
}