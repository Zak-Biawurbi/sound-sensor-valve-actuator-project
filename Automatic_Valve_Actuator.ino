#include <LiquidCrystal_I2C.h>
#include <Stepper.h>
#include <Wire.h>
// LCD screen
LiquidCrystal_I2C lcd(0x27, 16, 2);

// RGB LED pins
const int redPin = 5;
const int greenPin = 6;
const int bluePin = 7;

// Sound sensor and push button pins
const int soundSensorPin = 10;
const int soundThreshold = 100;
const int buttonPin = 9;

// Define stepper motor interface pins
const int dirPin = 2;
const int stepPin = 3;
const int enPin = 4;

// Define the number of steps per revolution
const int stepsPerRevolution = 1600;

// Define number of revolutions and speed
const int numberOfRevolutions = 3;
const int motorSpeed = 150;
Stepper stepper(stepsPerRevolution, dirPin, stepPin);

// Buzzer pin
const int buzzerPin = 8; // Changed to pin 8 to avoid conflict with emergencyButtonPin

// Other constants and variables
const unsigned long debounceDelay = 50;
volatile bool isEmergency = false;
volatile bool isStart = false;  // New variable to control start/stop
bool motorRunning = false;
bool clockwise = true;
unsigned long lastDebounceTime = 0;
bool rotationsCompleted = false;
volatile bool activeMode = false;
unsigned long startTime = 0;
unsigned long stopTime = 0;
int revolutionsCompleted = 0; // To keep track of the number of revolutions completed

void setup() {
  Serial.begin(9600);
  Serial.println("Setup started");
  // Initialize LCD screen
  lcd.init();
  lcd.backlight();
  lcd.begin(16, 2);
  lcd.print("WELCOME TO JK!!");
  lcd.setCursor(0, 1);
  lcd.print("Booster");
  delay(5000);
  lcd.clear();
  lcd.print("READY!!");

  // Initialize RGB LED pins
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);

  // Initialize sound sensor and push button pins
  pinMode(soundSensorPin, INPUT);
  pinMode(buttonPin, INPUT_PULLUP);
  
  // Initialize stepper motor speed in rpm
  stepper.setSpeed(motorSpeed);

  // Initialize buzzer pin
  pinMode(buzzerPin, OUTPUT);
  tone(buzzerPin, 4000);
  delay(1000);
  noTone(buzzerPin);
  delay(1000);
  
  Serial.println("Setup completed");
  
  // Attach interrupt to the push button pin
  attachInterrupt(digitalPinToInterrupt(buttonPin), emergencyStop, CHANGE);
  
  // Set system initial state
  digitalWrite(enPin, LOW);
  digitalWrite(bluePin, HIGH);
}

void loop() {
  int buttonState = digitalRead(buttonPin);
  int soundState = analogRead(soundSensorPin);
  if (buttonState == LOW && !activeMode && (millis() - lastDebounceTime) > debounceDelay) {  // Check if start button is pressed or sound is detected and not already in active mode
    enterActiveMode();
    Serial.println("System Started");
    lastDebounceTime = millis(); // Helps to avoid false reading from a switch
  }
  
  if (activeMode) {
    unsigned long currentTime = millis();
    if (currentTime - startTime < 10000) {
      blinkGreenLED();
    } else {
      lcd.clear();
      lcd.print("VALVE OPENING");
      setLEDColor(greenPin);
      digitalWrite(enPin, LOW);
      digitalWrite(dirPin, !clockwise ? HIGH : LOW);
      for (int i = revolutionsCompleted; i < numberOfRevolutions; i++) {
        if (digitalRead(buttonPin) == LOW && (millis() - lastDebounceTime) > debounceDelay) {  // Check if button is pressed again
          emergencyStop();
          if (digitalRead(buttonPin) == LOW && (millis() - lastDebounceTime) > debounceDelay) {
            lastDebounceTime = millis();
            if (clockwise) {
              lcd.clear();
              lcd.print("Valve Opening");
            }
            else {
              lcd.print("Valve Closing");
            }
            return;  // Exit loop to handle emergency
          }
        }
        rotateMotor(stepsPerRevolution, !clockwise);
        beepBuzzer(i + 1); // Beep according to the revolution number
        revolutionsCompleted = i + 1; // Update completed revolutions
      }
      activeMode = false;
      isStart = false;  // Reset start flag after operation ends
      lcd.clear();
      lcd.print("VALVE COMPLETELY");
      lcd.setCursor(0, 1);
      lcd.print("OPENED");
      setLEDColor(bluePin);
      delay(2000);
      lcd.clear();
      lcd.print("READY!!");
      digitalWrite(enPin, HIGH);
      clockwise = !clockwise; // Change direction after completion
      revolutionsCompleted = 0; // Reset revolutions for next direction
    }
  }
}

void enterActiveMode() {
  activeMode = true;
  startTime = millis();
  lcd.clear();
  lcd.print("ACTIVE MODE");
  blinkGreenLED();
}

void blinkGreenLED() {
  unsigned long currentTime = millis();
  if (currentTime - startTime < 10000) {
    if((currentTime - startTime) % 1000 < 500) {
      digitalWrite(greenPin, HIGH);
    }
    else {
      digitalWrite(greenPin, LOW);
    }
  }
  else {
    digitalWrite(greenPin, LOW);
  }
}

void rotateMotor(int steps, bool clockwise) {
  digitalWrite(dirPin, !clockwise ? HIGH : LOW);
  for (int i = 0; i < steps; i++) {
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(1000);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(1000);
  }
}
void beepBuzzer(int times) {
  for (int i = 0; i < times; i++) {
    tone(buzzerPin, 4000, 200);
    delay(300);
  }
}
void emergencyStop() {
  if (!isEmergency) {
    isEmergency = true;
    motorRunning = false;
    stopTime = millis();
    if (clockwise) {
      lcd.clear();
      lcd.print("Emergency Mode");
      lcd.setCursor(0, 1);
      lcd.print("Valve 1/2 Open");
    } 
    else {
      lcd.print("Valve 1/2 Shut");
    }
    // Disable motor
    digitalWrite(enPin, HIGH);
    // Beep buzzer for 2 seconds
    tone(buzzerPin, 4000);
    delay(2000);
    noTone(buzzerPin);
  }
  else {
    isEmergency = false;
    startTime += millis() - stopTime;
    motorRunning = true;
  }
}
void setLEDColor(int colorPin) {
  digitalWrite(redPin, LOW);
  digitalWrite(greenPin, LOW);
  digitalWrite(bluePin, LOW);
  digitalWrite(colorPin, HIGH);
}
