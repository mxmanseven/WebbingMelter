#include <Arduino.h>

#include <Wire.h>
#include <LiquidCrystal.h>

#include "Display.h"

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
Display display;

ButtonDirection buttonDirection;

void butLeftDown() {
  buttonDirection = ButtonDirection::Left;
}

void butRightDown() {
  buttonDirection = ButtonDirection::Down;
}

void butUpDown() {
  buttonDirection = ButtonDirection::Up;
}

void butDownDown() {
  buttonDirection = ButtonDirection::Down;
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
  
  display.UpdateDisplayAllRows(lcd);
  delay(1000);
}

void loop() 
{
  switch (topLevelMode) 
  {
    case TopLevelMode::SetUp:
    {
      display.UpdateDisplay(lcd, buttonDirection);
    }
    case TopLevelMode::Production:
    {
      
    }
  }

  if(buttonDirection != ButtonDirection::None) {
    Serial.println("button dir: " + String(buttonDirection));
    buttonDirection = ButtonDirection::None;
  }
}