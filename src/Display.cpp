#include "Display.h"

Display::Display() {
    Display::rowLabels[0] = Display::RUN_LABEL;
    Display::rowLabels[1] = Display::RUN_COUNT_LABEL;
    Display::rowLabels[2] = Display::MELT_SPEED_LABEL;
    Display::rowLabels[3] = Display::COOL_SPEED_LABEL;
    Display::rowLabels[4] = Display::CUT_LENGTH_LABEL;
    currentColumnIndex = 0;
    currentRowIndex = 0;
    currentLabelIndex = 0;
}

void Display::InitDisplay(LiquidCrystal& lcd) {    
    lcd.clear();
    Serial.println("Init display currentLableIndex:" + String(currentLabelIndex));

    for (int row = 0; row < Display::LCD_ROW_COUNT; row++){
        lcd.setCursor(0, row);
        int labelIndex = (currentLabelIndex + row) % Display::LABEL_COUNT;
        String toPrint = Display::rowLabels[labelIndex];
        Serial.println(
            "Init display rowLabels[" 
            + String(labelIndex) + "]: " +  toPrint); 
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
            break;
        }
        case ButtonDirection::Right:
        {
            break;            
        }
        case ButtonDirection::Up:
        {
            currentLabelIndex = (--currentLabelIndex < 0) 
                ? LABEL_COUNT - 1
                : --currentLabelIndex;
            currentRowIndex = (--currentRowIndex < 0)
                ? LCD_ROW_COUNT - 1
                : --currentRowIndex;
            break;
        }
        case ButtonDirection::Down:
        {
            currentLabelIndex = abs(++currentLabelIndex % LABEL_COUNT);
            currentRowIndex = abs(++currentRowIndex % LCD_ROW_COUNT);                
            break;
        }
        case ButtonDirection::None:
        {}
    }

    if(buttonDirection != ButtonDirection::None) {
        InitDisplay(lcd);
    }
}