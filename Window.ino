#include <ESP32Servo.h>

Servo clothesServo;
Servo windowServo;

int clothesPos = 0;
int clothesServoPin = 17;
int windowPos = 60;
int windowServoPin = 15;
int windowOpen = true;
int buttonPin = 5;
int buttonPressed = false;

void buttonISR() {
  buttonPressed = true;
}

void setup() {
  Serial.begin(9600);
	ESP32PWM::allocateTimer(0);
	ESP32PWM::allocateTimer(1);
	ESP32PWM::allocateTimer(2);
	ESP32PWM::allocateTimer(3);
	clothesServo.setPeriodHertz(50);
  clothesServo.write(clothesPos);
	clothesServo.attach(clothesServoPin, 500, 2500);
  windowServo.setPeriodHertz(50);
  windowServo.write(windowPos);
  windowServo.attach(windowServoPin, 500, 2500);
  attachInterrupt(digitalPinToInterrupt(buttonPin), buttonISR, FALLING);
}

void closeWindow() {
  // move clothesline in first
  for (; clothesPos <= 240; clothesPos += 1) {
    clothesServo.write(clothesPos);
    delay(2);
  }
  delay(500);
  
  // close window
  for (; windowPos <= 110; windowPos += 1) {
    windowServo.write(windowPos);
    delay(10);
  }
  windowOpen = false;
}

void openWindow() {
  if (!windowOpen) { 
    // open window first
    for (; windowPos >= 40; windowPos -= 1) {
      windowServo.write(windowPos);
      delay(10);
    }
    delay(500);

    // move clothesline out
    for (; clothesPos >= 0; clothesPos -= 1) {
      clothesServo.write(clothesPos);
      delay(2);
    }
    windowOpen = true;
  }
}

void loop() {
	if (buttonPressed) {
    buttonPressed = false;
	  if (windowOpen) {
	    closeWindow();
      Serial.println("Window close");
	  } else {
      openWindow();
      Serial.println("Window open");
	  }
    delay(2000);
	}
}
