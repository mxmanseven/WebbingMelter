#include "Display.h"

String FloatFormat(float f, uint8_t percision)
{
    char buff[10];
    int intValue = (int)f;
    snprintf(buff, 20, "%d", intValue);
    String result = String(buff);

    if(percision > 0)
    {
        float fractionPart = f - intValue;
        int multiplier =  pow(10, percision);
        int fractionPartInt = fractionPart * multiplier;

        char buffFraction[10];
        snprintf(buffFraction, 10, "%d", fractionPartInt);
        result += "." + String(buffFraction);
    }

    return result;

    // String f = FloatFormat(1.1, 0);
    // Serial.println(f);

    // f = FloatFormat(1.1, 1);
    // Serial.println(f);

    // f = FloatFormat(1.123, 3);
    // Serial.println(f);
}

SetupDisplay::SetupDisplay() {
    SetupDisplay::rowLabels[0] = SetupDisplay::RUN_LABEL;
    SetupDisplay::rowLabels[1] = SetupDisplay::RUN_COUNT_LABEL;
    SetupDisplay::rowLabels[2] = SetupDisplay::MELT_SPEED_LABEL;
    SetupDisplay::rowLabels[3] = SetupDisplay::COOL_SPEED_LABEL;
    SetupDisplay::rowLabels[4] = SetupDisplay::CUT_LENGTH_LABEL;
    currentColumnIndex = 0;
    currentRowIndex = 0;
    currentLabelIndex = 0;

    eeWrapper.Get(data);
    SetupDisplay::rowValues[0] = "Go";
    SetupDisplay::rowValues[1] = String(data.runCount);
    SetupDisplay::rowValues[2] = FloatFormat(data.meltSpeed, 2);
    SetupDisplay::rowValues[3] = FloatFormat(data.coolSpeed, 2);
    SetupDisplay::rowValues[4] = FloatFormat(data.cutLength, 3);
}

void SetupDisplay::ZeroOut(LiquidCrystal& lcd)
{
    lcd.clear();
    currentColumnIndex = 0;
    currentRowIndex = 0;
    currentLabelIndex = 0;
}

void SetupDisplay::UpdateDisplayAllRows(
    LiquidCrystal& lcd,
    bool& exitSetUpMode)
{
    //lcd.clear();  -- removing the clear increases lcd responsivness
    // make sure the whole screen is written to if clear is not used.
    Serial.println("Init display currentLableIndex:" + String(currentLabelIndex));

    for (int row = 0; row < SetupDisplay::LCD_ROW_COUNT; row++){
        lcd.setCursor(0, row);
        int labelIndex = (currentLabelIndex + row) % SetupDisplay::LABEL_COUNT;
        String toPrint = SetupDisplay::rowLabels[labelIndex];
        Serial.println(
            "Init display rowLabels[" 
            + String(labelIndex) + "]: " +  toPrint);        

        int lineBuffLength = 21;
        char lineBuff[lineBuffLength];

        String label = SetupDisplay::rowLabels[labelIndex];
        String value = SetupDisplay::rowValues[labelIndex];

        snprintf(lineBuff, lineBuffLength, "%-10s %9s",
            SetupDisplay::rowLabels[labelIndex].c_str(),
            SetupDisplay::rowValues[labelIndex].c_str());

        Serial.println(lineBuff);

        lcd.print(lineBuff);
    }
    
    char buff[10];
    snprintf(buff, 10, "%4.3f", data.cutLength);
    Serial.println("Display: " + String(buff));
}

void SetupDisplay::UpdateDisplay(
    LiquidCrystal& lcd, 
    ButtonDirection buttonDirection,
    bool& exitSetUpMode)
{
    switch (buttonDirection)
    {
        case ButtonDirection::Left:
        {
            currentColumnIndex = 0;
            break;
        }
        case ButtonDirection::Right:
        {
            currentColumnIndex = 1;
            break;            
        }
        case ButtonDirection::Up:
        {
            if(currentColumnIndex == 0)
            {
                currentLabelIndex = (--currentLabelIndex < 0) 
                    ? LABEL_COUNT - 1
                    : --currentLabelIndex;
                currentRowIndex = (--currentRowIndex < 0)
                    ? LCD_ROW_COUNT - 1
                    : --currentRowIndex;
            }
            else
            {
                ChangeRowValue(currentRowIndex, buttonDirection, exitSetUpMode);
            }
            
            break;
        }
        case ButtonDirection::Down:
        {
            if(currentColumnIndex == 0)
            {
                currentLabelIndex = abs(++currentLabelIndex % LABEL_COUNT);
                currentRowIndex = abs(++currentRowIndex % LCD_ROW_COUNT);
            }
            else
            {
                ChangeRowValue(currentRowIndex, buttonDirection, exitSetUpMode);
            }

            break;
        }
        case ButtonDirection::None:
        {}
    }

    if(buttonDirection != ButtonDirection::None) {
        UpdateDisplayAllRows(lcd, exitSetUpMode);
    }
}

void SetupDisplay::ChangeRowValue(
            int rowIndex,
            ButtonDirection direction,
            bool& exitSetUpMode)
{
    if(rowIndex == 0) {
        // this is the run go row, if button is right then enter production mode
        exitSetUpMode = true;        
    }
    else {
        String rowValueStr = SetupDisplay::rowValues[rowIndex];
        if (rowIndex >= 2 || rowIndex <= 4) {
            // row value is a float
            float rowValueF = rowValueStr.toFloat();
            float changeAmount = 0.01;
            if(direction == ButtonDirection::Down) changeAmount = changeAmount * -1;
            rowValueF += changeAmount;
        }
    }
}