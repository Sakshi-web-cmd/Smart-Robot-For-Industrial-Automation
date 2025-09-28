/*
 * Multipurpose Robot
 * Features:
 * - Obstacle Avoidance (Ultrasonic + Servo)
 * - Fire Detection (Flame Sensor + Water Pump)
 * - Manual Bluetooth Control
 *
 * Hardware:
 * - Arduino + Adafruit Motor Shield
 * - 4 DC Motors
 * - Ultrasonic Sensor (HC-SR04)
 * - Flame Sensor
 * - Water Pump
 * - Servo Motor for Ultrasonic rotation
 * - Bluetooth Module (HC-05/06)
 */

#include <Servo.h>
#include <AFMotor.h>

// ---------------- Pin Definitions ----------------
#define Echo A0
#define Trig A1
#define motor 10
#define Speed 170
#define spoint 103

#define FlameSensor A2
#define WaterPump 9   // Change to A3 if connected on analog pin A3

// ---------------- Bluetooth Commands ----------------
const char CMD_FORWARD  = 'F';
const char CMD_BACKWARD = 'B';
const char CMD_LEFT     = 'L';
const char CMD_RIGHT    = 'R';
const char CMD_STOP     = 'S';

char value;
int distance;
int flameValue;

Servo servo;
AF_DCMotor M1(1);
AF_DCMotor M2(2);
AF_DCMotor M3(3);
AF_DCMotor M4(4);

// ---------------- Setup ----------------
void setup() {
  Serial.begin(9600);

  pinMode(Trig, OUTPUT);
  pinMode(Echo, INPUT);
  pinMode(FlameSensor, INPUT);
  pinMode(WaterPump, OUTPUT);

  servo.attach(motor);

  M1.setSpeed(Speed);
  M2.setSpeed(Speed);
  M3.setSpeed(Speed);
  M4.setSpeed(Speed);

  digitalWrite(WaterPump, LOW); // Pump OFF initially
}

// ---------------- Loop ----------------
void loop() {
  FireDetection();   // Fire detection
  Obstacle();        // Obstacle avoidance

  // Uncomment below if you want manual control instead
  // BluetoothControl();
}

// ---------------- Functions ----------------

// Manual Bluetooth control
void BluetoothControl() {
  if (Serial.available() > 0) {
    value = Serial.read();
    Serial.println(value);
    switch (value) {
      case CMD_FORWARD:  forward(); break;
      case CMD_BACKWARD: backward(); break;
      case CMD_LEFT:     left(); break;
      case CMD_RIGHT:    right(); break;
      case CMD_STOP:     Stop(); break;
    }
  }
}

// Obstacle avoidance using ultrasonic
void Obstacle() {
  distance = ultrasonic();
  if (distance <= 12) {
    Stop();
    backward();
    delay(100);
    Stop();

    int leftDistance = leftsee();
    servo.write(spoint);
    delay(800);

    int rightDistance = rightsee();
    servo.write(spoint);

    if (leftDistance < rightDistance) {
      right();
      delay(500);
      Stop();
      delay(200);
    } else {
      left();
      delay(500);
      Stop();
      delay(200);
    }
  } else {
    forward();
  }
}

// Fire detection with flame sensor
void FireDetection() {
  flameValue = analogRead(FlameSensor);
  if (flameValue < 400) { // Adjust threshold based on your sensor
    Serial.println("🔥 Fire detected! Spraying water...");
    Stop();
    sprayWater();
    delay(2000);
  }
}

// Water pump control
void sprayWater() {
  digitalWrite(WaterPump, HIGH); // Pump ON
  delay(2000);                   // Spray for 2 sec
  digitalWrite(WaterPump, LOW);  // Pump OFF
}

// Ultrasonic distance measurement
int ultrasonic() {
  digitalWrite(Trig, LOW);
  delayMicroseconds(4);
  digitalWrite(Trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(Trig, LOW);

  long t = pulseIn(Echo, HIGH);
  long cm = t / 29 / 2;
  return cm;
}

// ---------------- Motor Controls ----------------
void forward() {
  M1.run(FORWARD);
  M2.run(FORWARD);
  M3.run(FORWARD);
  M4.run(FORWARD);
}

void backward() {
  M1.run(BACKWARD);
  M2.run(BACKWARD);
  M3.run(BACKWARD);
  M4.run(BACKWARD);
}

void right() {
  M1.run(BACKWARD);
  M2.run(BACKWARD);
  M3.run(FORWARD);
  M4.run(FORWARD);
}

void left() {
  M1.run(FORWARD);
  M2.run(FORWARD);
  M3.run(BACKWARD);
  M4.run(BACKWARD);
}

void Stop() {
  M1.run(RELEASE);
  M2.run(RELEASE);
  M3.run(RELEASE);
  M4.run(RELEASE);
}

// ---------------- Servo Scanning ----------------
int rightsee() {
  servo.write(20);
  delay(800);
  return ultrasonic();
}

int leftsee() {
  servo.write(180);
  delay(800);
  return ultrasonic();
}