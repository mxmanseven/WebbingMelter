#include "Display.h"

Display::Display() {
    Display::rowLabels[0] = Display::RUN_LABEL;
    Display::rowLabels[1] = Display::RUN_COUNT_LABEL;
    Display::rowLabels[2] = Display::MELT_SPEED_LABEL;
    Display::rowLabels[3] = Display::COOL_SPEED_LABEL;
    currentColumnIndex = 0;
    currentRowIndex = 0;
}

void Display::InitDisplay(LiquidCrystal& lcd) {    
    lcd.clear();

    for (int row = 0; row < Display::LCD_ROW_COUNT; row++){
        lcd.setCursor(0, row);
        int labelIndex = (currentRowIndex + row) % Display::LCD_ROW_COUNT;
        Serial.println("init display: label index: " + String(labelIndex));
        String toPrint = Display::rowLabels[labelIndex];
        Serial.println("Init display label to print: " +  toPrint); 
        lcd.print(Display::rowLabels[labelIndex]);    
    }
}

void Display::UpdateDisplay(
    LiquidCrystal& lcd, 
    ButtonDirection buttonDirection) {
    switch (buttonDirection)
    {
        case ButtonDirection::Left:
        {

        }
        case ButtonDirection::Right:
        {
            
        }
        case ButtonDirection::Up:
        {
            currentRowIndex = abs(--currentRowIndex % LCD_ROW_COUNT);
        }
        case ButtonDirection::Down:
        {
            currentRowIndex = abs(++currentRowIndex % LCD_ROW_COUNT);                
        }
    }

    if(buttonDirection != ButtonDirection::None) {
        InitDisplay(lcd);
    }
}