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

     eeWrapper.Get(data);
    Display::rowValues[0] = "Go";
    Display::rowValues[1] = String(data.runCount);
    Display::rowValues[2] = String(data.meltSpeed);
    Display::rowValues[3] = String(data.coolSpeed);
    Display::rowValues[4] = String(data.cutLength);
}

void Display::UpdateDisplayAllRows(LiquidCrystal& lcd) {    
    //lcd.clear();  -- removing the clear increases lcd responsivness
    // make sure the whole screen is written to if clear is not used.
    Serial.println("Init display currentLableIndex:" + String(currentLabelIndex));

    for (int row = 0; row < Display::LCD_ROW_COUNT; row++){
        lcd.setCursor(0, row);
        int labelIndex = (currentLabelIndex + row) % Display::LABEL_COUNT;
        String toPrint = Display::rowLabels[labelIndex];
        Serial.println(
            "Init display rowLabels[" 
            + String(labelIndex) + "]: " +  toPrint); 
        


        int lineBuffLength = 21;
        char lineBuff[lineBuffLength];

        String label = Display::rowLabels[labelIndex];
        String value = Display::rowValues[labelIndex];

        Serial.println(label);
        Serial.println(value);

        snprintf(lineBuff, lineBuffLength, "%-10s %9s",
            Display::rowLabels[labelIndex].c_str(),
            Display::rowValues[labelIndex].c_str());

        Serial.println(lineBuff);

        lcd.print(lineBuff);
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
        UpdateDisplayAllRows(lcd);
    }
}