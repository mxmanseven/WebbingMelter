#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal.h>
#include "BasicStepperDriver.h"
#include "Display.h"
#include "DisplayProd.h"

const int X_DEGREES_PER_INCH = 185;
const int Y_DEGREES_PER_INCH = 2215;
const int X_RPM_STD = 80;
const float X_RPM_MELT = 5;
const int Y_RPM = 300;

const float X_SAFE_TO_HOT_COOL_PLATE_DEGREES = X_DEGREES_PER_INCH * 0.5;
const float X_HOT_COOL_PLATE_TO_CUT_DEGREES = X_DEGREES_PER_INCH * 0.5;
float X_MELT_DEGREES = 0; // set in setup()
float X_CUT_LENGTH_DEGREES = 0; // set in setup()

const int Y_TOPOUT_TO_MELT_DEGREES = Y_DEGREES_PER_INCH * 3.0;
const int Y_MELT_TO_CUT_DEGREES = Y_DEGREES_PER_INCH * 1.0;
const int Y_CUT_TO_COOL_DEGREES = Y_DEGREES_PER_INCH * 1.0;
const int Y_TOPOUT_EXTRA_DEGREES = Y_DEGREES_PER_INCH * 0.1;

const int CUT_RELAY_DURATION_MS = 1500;
const int RELAY_ROLL_DURATION_MS = 3000;
const int RELAY_AIR_DURATION_MS = 1000;

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
const uint8_t RELAY_WHEEL_PIN = PB9;

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
}

void pullButtonStatus(ButtonDirection& buttonDirection) {
  if (digitalRead(btnLeftPin) == HIGH) {
    buttonDirection = ButtonDirection::Left;
    return;
  }
  if (digitalRead(btnRightPin) == HIGH) {
    buttonDirection = ButtonDirection::Right;
    return;
  }
  if (digitalRead(btnUpPin) == HIGH) {
    buttonDirection = ButtonDirection::Up;
    return;
  }
  if (digitalRead(btnDownPin) == HIGH) {
    buttonDirection = ButtonDirection::Down;
    return;
  }
  buttonDirection = ButtonDirection::None; 
}

void initRelay() {
  pinMode(RELAY_CUT_PIN, OUTPUT);
  pinMode(RELAY_ROLL_PIN, OUTPUT);
  pinMode(RELAY_AIR_PIN, OUTPUT);
  pinMode(RELAY_WHEEL_PIN, OUTPUT);

  digitalWrite(RELAY_CUT_PIN, HIGH);
  digitalWrite(RELAY_ROLL_PIN, HIGH);
  digitalWrite(RELAY_AIR_PIN, HIGH);
  digitalWrite(RELAY_WHEEL_PIN, HIGH);
}

void initStepper()  {
  stepperX.begin(X_RPM_STD, MICROSTEPS);
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

void doProductionCycle() {
  int d = 1;

  // zero out on top plate
  Serial.println("zero out on top plate");
  displayProd.SetCommand(lcd, "ZERO OUT TOP PLATE   ");
  stepperY.rotate(
      Y_TOPOUT_TO_MELT_DEGREES 
      + Y_MELT_TO_CUT_DEGREES
      + Y_CUT_TO_COOL_DEGREES 
      + Y_TOPOUT_EXTRA_DEGREES);
  delay(d);
  
  // move down to cut
  Serial.println("move down to cut");
  displayProd.SetCommand(lcd, "MOVE DOWN TO CUT    ");
  stepperY.rotate(-1 * (Y_TOPOUT_TO_MELT_DEGREES + Y_MELT_TO_CUT_DEGREES));
  delay(d);

  // Open Wheels
  Serial.println("Open Wheels");
  digitalWrite(RELAY_WHEEL_PIN, LOW);
  delay(1000);

  // feed for cut from safe position
  Serial.println("feed for cut from safe position");
  displayProd.SetCommand(lcd, "FEED FOR CUT        ");
  stepperX.rotate(X_SAFE_TO_HOT_COOL_PLATE_DEGREES + X_HOT_COOL_PLATE_TO_CUT_DEGREES + X_CUT_LENGTH_DEGREES);
  //delay(10000);
  // un-do for now
  // knh todo - remove when in prod
  Serial.println("un-do for now");
  stepperX.rotate(-1 * (X_SAFE_TO_HOT_COOL_PLATE_DEGREES + X_HOT_COOL_PLATE_TO_CUT_DEGREES + X_CUT_LENGTH_DEGREES));
  delay(d);

  // relay cut
  Serial.println("relay cut");
  displayProd.SetCommand(lcd, "RELAY CUT           ");
  digitalWrite(RELAY_CUT_PIN, LOW);
  delay(CUT_RELAY_DURATION_MS);
  digitalWrite(RELAY_CUT_PIN, HIGH);
  delay(1000);

  // Close Wheels
  Serial.println("Close Wheels");
  digitalWrite(RELAY_WHEEL_PIN, HIGH);
  delay(1000);

  // Roll
  Serial.println("Roll");
  displayProd.SetCommand(lcd, "RELAY ROLL          ");
  digitalWrite(RELAY_ROLL_PIN, LOW);
  delay(RELAY_ROLL_DURATION_MS);
  digitalWrite(RELAY_ROLL_PIN, HIGH);

  // Air
  Serial.println("Air");
  displayProd.SetCommand(lcd, "RELAY AIR           ");
  digitalWrite(RELAY_AIR_PIN, LOW);
  delay(RELAY_AIR_DURATION_MS);
  digitalWrite(RELAY_AIR_PIN, HIGH);

  // retract back to safety
  Serial.println("retract back to safety");
  displayProd.SetCommand(lcd, "RETRACT TO SAFETY   ");
  stepperX.rotate(-1 * (X_HOT_COOL_PLATE_TO_CUT_DEGREES + X_SAFE_TO_HOT_COOL_PLATE_DEGREES));
  delay(d);

  // up to melt
  Serial.println("up to melt");
  displayProd.SetCommand(lcd, "MOVE UP TO MELT     ");
  stepperY.rotate(Y_MELT_TO_CUT_DEGREES);
  delay(5000);

  // feed to melt
  Serial.println("feed to melt inches:" + String(X_SAFE_TO_HOT_COOL_PLATE_DEGREES));
  displayProd.SetCommand(lcd, "FEED TO MELT        ");
  stepperX.setRPM(X_RPM_STD);
  stepperX.rotate(X_SAFE_TO_HOT_COOL_PLATE_DEGREES);
  delay(10000);
  
  Serial.println("melt slow degrees: " + String(X_MELT_DEGREES));
  displayProd.SetCommand(lcd, "SLOW TO MELT        ");
  stepperX.setRPM(X_RPM_MELT);
  stepperX.rotate(X_MELT_DEGREES);
  stepperX.setRPM(X_RPM_STD);
  delay(10000);
  
  // retract to safety
  Serial.println("retract to safety");
  displayProd.SetCommand(lcd, "RETRACT TO SAFETY   ");
  stepperX.rotate(-1 * (X_SAFE_TO_HOT_COOL_PLATE_DEGREES));
  delay(d);

  // down to cool
  Serial.println("down to cool");
  displayProd.SetCommand(lcd, "MOVE DOWN TO COOL   ");
  stepperY.rotate(-1 * (Y_MELT_TO_CUT_DEGREES + Y_CUT_TO_COOL_DEGREES));
  delay(5000);

  // feed to cool
  Serial.println("feed to cool");
  displayProd.SetCommand(lcd, "FEED TO COOL        ");
  stepperX.rotate(X_SAFE_TO_HOT_COOL_PLATE_DEGREES);
  delay(5000);

  // retract to safety
  Serial.println("retract to safety");
  displayProd.SetCommand(lcd, "RETRACT TO SAFETY   ");
  stepperX.rotate(-1 * (X_SAFE_TO_HOT_COOL_PLATE_DEGREES));
   delay(5000);
}

void loop() 
{
  if (topLevelMode == TopLevelMode::SetUp) {
    pullButtonStatus(buttonDirection);
    if(buttonDirection != ButtonDirection::None) delay(200);
  }

  if(topLevelMode == TopLevelMode::SetUp 
    && buttonDirection != ButtonDirection::None) {      
      bool exitSetUpMode = false;
      setupDisplay.UpdateDisplay(lcd, buttonDirection, exitSetUpMode);
      
      if (exitSetUpMode) {
        topLevelMode = TopLevelMode::Production;

        setupDisplay.ZeroOut(lcd);

        int runCount = setupDisplay.data.runCount;
        displayProd.expectedRunCount = runCount;
        displayProd.currentRunCount = 0;

        displayProd.ShowRunCount(lcd);
        displayProd.SetCommand(lcd, "Prod Mode");
      }
    Serial.println("button dir: " + String(buttonDirection));
    buttonDirection = ButtonDirection::None;
  }  

  if (topLevelMode == TopLevelMode::Production)
  {
    // get values from lcd
    X_MELT_DEGREES = X_DEGREES_PER_INCH * setupDisplay.data.meltDist;
    X_CUT_LENGTH_DEGREES = X_DEGREES_PER_INCH * setupDisplay.data.cutLength;
  
    displayProd.expectedRunCount = setupDisplay.data.runCount;
    displayProd.currentRunCount = 0;
    lcd.noCursor();

    for(int i = 0; i < setupDisplay.data.runCount; i++) {
      doProductionCycle();
      displayProd.currentRunCount = i + 1;
      displayProd.ShowRunCount(lcd);
      setupDisplay.data.lifeTimerunCount++;
      setupDisplay.WritePersistantDate();
    }

    topLevelMode = TopLevelMode::SetUp;
    initRelay();

    setupDisplay.rowValues[setupDisplay.LIFETIME_RUN_LABLE_INDEX] 
      = String(setupDisplay.data.lifeTimerunCount);
    bool exitSetUpMode = false;
    setupDisplay.UpdateDisplayAllRows(lcd, exitSetUpMode);
    lcd.cursor();
  }
}