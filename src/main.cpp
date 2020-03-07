#include <Arduino.h>

#include <Wire.h>
#include <LiquidCrystal.h>

#include "BasicStepperDriver.h"

#include "Display.h"
#include "DisplayProd.h"


const int X_DEGREES_PER_INCH = 185;
const int Y_DEGREES_PER_INCH = 2215;
const int X_RPM_STD = 60;
const int X_RPM_MELT = 10;
const int Y_RPM = 300;


enum TopLevelMode 
{
  SetUp,
  Production
};

enum ProductionMode
{
  ZeroOut,
  Run
};

TopLevelMode topLevelMode;
ProductionMode productionMode;

// Motor steps per revolution. Most steppers are 200 steps or 1.8 degrees/step
const int MOTOR_STEPS = 200;

// Since microstepping is set externally, make sure this matches the selected mode
// If it doesn't, the motor will move at a different RPM than chosen
// 1=full step, 2=half step etc.
const int MICROSTEPS = 8;

#define STEPPER_X_STEP_PIN PB12
#define STEPPER_X_DIR_PIN PB13

#define STEPPER_Y_STEP_PIN PB14
#define STEPPER_Y_DIR_PIN PB15

BasicStepperDriver stepperX(MOTOR_STEPS, STEPPER_X_DIR_PIN, STEPPER_X_STEP_PIN);
BasicStepperDriver stepperY(MOTOR_STEPS, STEPPER_Y_DIR_PIN, STEPPER_Y_STEP_PIN);

const uint8_t RELAY_CUT_PIN = PB6;
const uint8_t RELAY_ROLL_PIN = PB7;
const uint8_t RELAY_AIR_PIN = PB8;

// nav button pins:
uint8_t btnLeftPin = PB10;
uint8_t btnRightPin = PA7;
uint8_t btnUpPin = PB1;
uint8_t btnDownPin = PB0;

// LCD PINS
uint8_t lcdRs = PA5;
uint8_t lcdEnable = PA4;
uint8_t lcdD0 = PA3;
uint8_t lcdD1 = PA2;
uint8_t lcdD2 = PA1;
uint8_t lcdD3 = PA0;

LiquidCrystal lcd(lcdRs, lcdEnable, lcdD0, lcdD1, lcdD2, lcdD3);
SetupDisplay setupDisplay;
DisplayProd displayProd;

ButtonDirection buttonDirection;

void butLeftDown() {
  noInterrupts();
  buttonDirection = ButtonDirection::Left;
  interrupts();
}

void butRightDown() {
  noInterrupts();
  buttonDirection = ButtonDirection::Right;
  interrupts();
}

void butUpDown() {
  noInterrupts();
  buttonDirection = ButtonDirection::Up;
  interrupts();
}

void butDownDown() {
  noInterrupts();
  buttonDirection = ButtonDirection::Down;
  interrupts();
}

void initSerial() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
}

void initButtons() {
  pinMode(btnLeftPin, INPUT_PULLDOWN);
  pinMode(btnRightPin, INPUT_PULLDOWN);
  pinMode(btnUpPin, INPUT_PULLDOWN);
  pinMode(btnDownPin, INPUT_PULLDOWN);

  attachInterrupt(btnLeftPin, butLeftDown, RISING);
  attachInterrupt(btnRightPin, butRightDown, RISING);
  attachInterrupt(btnUpPin, butUpDown, RISING);
  attachInterrupt(btnDownPin, butDownDown, RISING);
}

void initRelay() {
  pinMode(RELAY_CUT_PIN, OUTPUT);
  pinMode(RELAY_ROLL_PIN, OUTPUT);
  pinMode(RELAY_AIR_PIN, OUTPUT);

  digitalWrite(RELAY_CUT_PIN, HIGH);
  digitalWrite(RELAY_ROLL_PIN, HIGH);
  digitalWrite(RELAY_AIR_PIN, HIGH);
  
  // digitalWrite(RELAY_CUT_PIN, LOW);
  // digitalWrite(RELAY_ROLL_PIN, LOW);
  // digitalWrite(RELAY_AIR_PIN, LOW);
}

void initStepper()  {
  stepperX.begin(X_RPM_MELT, MICROSTEPS);
  stepperY.begin(Y_RPM, MICROSTEPS);
}

// the setup function runs once when you press reset or power the board
void setup() {
  topLevelMode = TopLevelMode::SetUp;
  buttonDirection = ButtonDirection::None;
  initSerial();
  initButtons();
  initRelay();
  initStepper();

  lcd.begin(20, 4);
  lcd.cursor();
  bool exitSetUpMode = false;
  setupDisplay.UpdateDisplayAllRows(lcd, exitSetUpMode);
  delay(1000);
}

void loop() 
{
  if(buttonDirection != ButtonDirection::None) {
    switch (topLevelMode) 
    {
      case TopLevelMode::SetUp:
      {      
        bool exitSetUpMode = false;
        setupDisplay.UpdateDisplay(lcd, buttonDirection, exitSetUpMode);
        
        if (exitSetUpMode) {
          topLevelMode = TopLevelMode::Production;

          setupDisplay.ZeroOut(lcd);

          int runCount = setupDisplay.data.runCount;
          displayProd.expectedRunCount = runCount;
          displayProd.currentRunCount = 0;

          displayProd.ShowCommandAndRunCount(lcd, "Prod Mode");
        }
        break;
      }
      case TopLevelMode::Production:
      {
        break;
      }
    }

    Serial.println("button dir: " + String(buttonDirection));
    buttonDirection = ButtonDirection::None;
  }

  // Serial.println("x rotate 100");
  // stepperX.rotate(360);
  
//   Serial.println("y rotate 10");
//   stepperY.rotate(3600);

//  delay(10000);
  
//   // Serial.println("x rotate -100");
//   // stepperX.rotate(-360);
  
//   Serial.println("y rotate -10");
//   stepperY.rotate(-3600);

//  delay(10000);

  int air = digitalRead(RELAY_AIR_PIN);
  int cut = digitalRead(RELAY_CUT_PIN);
  int roll = digitalRead(RELAY_ROLL_PIN);

  Serial.println("air: " + String(air) + " cut: " + String(cut) + " roll: " + String(roll));

  delay(100);
  digitalWrite(RELAY_CUT_PIN, LOW);
  delay(1000);
  Serial.println("Cut enabled");
  digitalWrite(RELAY_CUT_PIN, HIGH);
  
  delay(100);
  digitalWrite(RELAY_ROLL_PIN, LOW);
  delay(1000);
  Serial.println("Roll enabled");
  digitalWrite(RELAY_ROLL_PIN, HIGH);

  delay(100);
  digitalWrite(RELAY_AIR_PIN, LOW);
  delay(1000);
  Serial.println("Air enabled");
  digitalWrite(RELAY_AIR_PIN, HIGH);
}