#include <Arduino.h>

#include <Wire.h>
#include <LiquidCrystal.h>

#include "BasicStepperDriver.h"

#include "Display.h"
#include "DisplayProd.h"

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
#define MOTOR_STEPS 200
#define RPM 100

// Since microstepping is set externally, make sure this matches the selected mode
// If it doesn't, the motor will move at a different RPM than chosen
// 1=full step, 2=half step etc.
#define MICROSTEPS 8

// All the wires needed for full functionality
#define DIR PB12
#define STEP PB13
//Uncomment line to use enable/disable functionality
//#define SLEEP 13

// 2-wire basic config, microstepping is hardwired on the driver
BasicStepperDriver stepper(MOTOR_STEPS, DIR, STEP);


// nav button pins:
uint8_t btnLeftPin = PB10;
uint8_t btnRightPin = PA7;
uint8_t btnUpPin = PB1;
uint8_t btnDownPin = PB0;

// PB0 AND PA7 TRIGGER THE SAME ISR
// DOWN AND RIGHT TRIGGER THE SAME ISR

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

// the setup function runs once when you press reset or power the board
void setup() {
  topLevelMode = TopLevelMode::SetUp;
  buttonDirection = ButtonDirection::None;
  initSerial();
  initButtons();
  lcd.begin(20,4);
  lcd.cursor();
  
  bool exitSetUpMode = false;
  setupDisplay.UpdateDisplayAllRows(lcd, exitSetUpMode);
  delay(1000);

  
  stepper.begin(RPM, MICROSTEPS);
}

void loop() 
{

  stepper.setRPM(100);
  Serial.println("stepper.rotate(360)");
  stepper.rotate(360);

  
  Serial.println("delay(5000)");
  delay(1000);

  stepper.setRPM(1);
  Serial.println("stepper.rotate(360)");
  stepper.rotate(30);

  // Serial.println("stepper.move()");
  // stepper.move(-MOTOR_STEPS*MICROSTEPS);

  Serial.println("delay(5000)");
  delay(1000);


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
}