#include <Arduino.h>

#include <Wire.h>
#include <LiquidCrystal.h>

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

bool btnLeftIsDown = false;
bool btnRightIsDown = false;
bool btnUpIsDown = false;
bool btnDownIsDown = false;

void butLeftDown()
{
  btnLeftIsDown = true;
}

void butRightDown()
{
  btnRightIsDown = true;
}

void butUpDown()
{
  btnUpIsDown = true;
}
void butDownDown()
{
  btnDownIsDown = true;
}

void initSerial()
{
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
}

void initButtons()
{
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

  initSerial();
  initButtons();
  lcd.begin(20,4);               // initialize the lcd
}

int i = 0;
void loop() 
{
  Serial.println("hi" + String(i++));
  delay(1000);

  lcd.clear();

  char buff[22];

  lcd.setCursor(0,0);
  //String(i).toCharArray(buff, 10, 0);
  snprintf(buff, 22, "Run Count %10d", i);
  lcd.print(buff);

  lcd.setCursor(0,1);
  lcd.print("Two");

  lcd.setCursor(0,2);
  lcd.print("Three");

  lcd.setCursor(0,3);
  lcd.print("Four");

  if(btnRightIsDown)
  {
    Serial.println("Btn Right Down");
    btnRightIsDown = false;
  }

  if(btnLeftIsDown)
  {
    Serial.println("Btn Left is donw");
    btnLeftIsDown = false;
  }
  
  if(btnUpIsDown)
  {
    Serial.println("Btn Up Down");
    btnUpIsDown = false;
  }

  if(btnDownIsDown)
  {
    Serial.println("Btn Down is down");
    btnDownIsDown = false;
  }
}