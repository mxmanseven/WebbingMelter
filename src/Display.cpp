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

        String filler = ".";
        if(fractionPartInt < multiplier / 10) { // need to add leading zeros
            filler += "0";
        }

        char buffFraction[10];
        snprintf(buffFraction, 10, "%d", fractionPartInt);
        result += filler + String(buffFraction);
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
    SetupDisplay::rowLabels[1] = SetupDisplay::LIFETIME_RUN_LABEL;
    SetupDisplay::rowLabels[2] = SetupDisplay::RUN_COUNT_LABEL;
    SetupDisplay::rowLabels[3] = SetupDisplay::MELT_DIST_LABEL;
    SetupDisplay::rowLabels[4] = SetupDisplay::CUT_LENGTH_LABEL;
    currentColumnIndex = 0;
    currentLabelIndex = 0;

    //eeWrapper.Test();

    eeWrapper.Get(data);
    SetupDisplay::rowValues[0] = "GO";
    SetupDisplay::rowValues[1] = String(data.lifeTimerunCount);
    SetupDisplay::rowValues[2] = String(data.runCount);
    SetupDisplay::rowValues[3] = FloatFormat(data.meltDist, 2);
    SetupDisplay::rowValues[4] = FloatFormat(data.cutLength, 2);
}

void SetupDisplay::ZeroOut(LiquidCrystal& lcd)
{
    lcd.clear();
    currentColumnIndex = 0;
    currentLabelIndex = 0;
}

void SetupDisplay::UpdateDisplayAllRows(
    LiquidCrystal& lcd,
    bool& exitSetUpMode)
{
    //lcd.clear();  -- removing the clear increases lcd responsivness
    // make sure the whole screen is written to if clear is not used.

    for (int row = 0; row < SetupDisplay::LCD_ROW_COUNT; row++){
        lcd.setCursor(0, row);
        int labelIndex = (currentLabelIndex + row) % SetupDisplay::LABEL_COUNT;
        String toPrint = SetupDisplay::rowLabels[labelIndex];

        int lineBuffLength = 21;
        char lineBuff[lineBuffLength];

        String label = SetupDisplay::rowLabels[labelIndex];
        String value = SetupDisplay::rowValues[labelIndex];

        snprintf(lineBuff, lineBuffLength, "%-14s %5s",
            SetupDisplay::rowLabels[labelIndex].c_str(),
            SetupDisplay::rowValues[labelIndex].c_str());

        Serial.println(lineBuff);

        lcd.print(lineBuff);
    }

    
    Serial.println("");
    
    // set the cursor to the left when changing nav labels
    // set the cursor to the right when changing values (+/-)
    int column = currentColumnIndex == 0 ? 0 : 19;
    lcd.setCursor(column, 0);
    // char buff[10];
    // snprintf(buff, 10, "%4.3f", data.cutLength);
    // Serial.println("Display: " + String(buff));
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
            currentColumnIndex = 0; // nav labels, up / down -> change menu option
            break;
        }
        case ButtonDirection::Right:
        {
            currentColumnIndex = 1;  // nav values, up / down -> change menu value
            break;            
        }
        case ButtonDirection::Up:
        {
            Serial.println("Current Lable Index init:" + String(currentLabelIndex));

            if(currentColumnIndex == 0) // change menu option
            {
                currentLabelIndex--;
                if(currentLabelIndex < 0) {
                    currentLabelIndex = LABEL_COUNT - 1;
                }

                Serial.println("Current Lable Index end :" + String(currentLabelIndex));

                // currentLabelIndex = (--currentLabelIndex < 0) // wrap around to last item if we go past first itme
                //     ? LABEL_COUNT - 1
                //     : --currentLabelIndex;
            }
            else
            {
                ChangeRowValue(currentLabelIndex, buttonDirection, exitSetUpMode);
            }
            
            break;
        }
        case ButtonDirection::Down:
        {
            if(currentColumnIndex == 0) // change menu option
            {
                currentLabelIndex = abs(++currentLabelIndex % LABEL_COUNT);
            }
            else
            {
                ChangeRowValue(currentLabelIndex, buttonDirection, exitSetUpMode);
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
            int labelIndex,
            ButtonDirection direction,
            bool& exitSetUpMode)
{
    if(labelIndex == 0) {
        // this is the run go row, 
        //  if button is right then enter production mode
        exitSetUpMode = true;        
    }
    else {
        String rowValueStr = SetupDisplay::rowValues[labelIndex];
        if (labelIndex >= RUN_COUNT_LABLE_INDEX && labelIndex <= CUT_LABLE_INDEX) {
            // row value is a float
            float rowValueF = rowValueStr.toFloat();
            // use goofy values because of rounding and precision limits.
            float changeAmount = 0.01001;
            if(direction == ButtonDirection::Down) changeAmount = -0.0099;

            if(labelIndex == RUN_COUNT_LABLE_INDEX) changeAmount = changeAmount * 1000;
            rowValueF += changeAmount;
            rowValueStr = FloatFormat(rowValueF, 2);
            SetupDisplay::rowValues[labelIndex] = rowValueStr;

            switch (labelIndex)
            {
                case 2: // run count index
                {
                    data.runCount = (int)rowValueF;
                    SetupDisplay::rowValues[labelIndex] = String(data.runCount);
                    break;
                }
                case 3: // melt index
                {
                    data.meltDist = rowValueF;
                    break;
                }
                // case 3: // cool index
                // {
                //     data.coolSpeed = rowValueF;
                //     break;
                // }
                case 4: // cut length index
                {
                    data.cutLength = rowValueF;
                    break;
                }
            }

            //eeWrapper.Write(data);
            WritePersistantDate();
        }
    }
}

void SetupDisplay::WritePersistantDate() { 
    eeWrapper.Write(data);
}