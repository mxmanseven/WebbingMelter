#ifndef DISPLAY_H_
#define DISPLAY_H_

#include <Wire.h>
#include <LiquidCrystal.h>
#include "EeWrapper.h"

enum ButtonDirection {
    None,   // 0
    Left,   // 1
    Right,  // 2
    Up,     // 3
    Down    // 4
};

String FloatFormat(float f, uint8_t percision);

class SetupDisplay
{
    public:
        const int LCD_COLUMN_COUNT = 20;
        const int LCD_ROW_COUNT = 4;
        static const int LABEL_COUNT = 5;

        const char* RUN_LABEL = "Run";
        const char* RUN_COUNT_LABEL =  "Run Count";
        const char* MELT_SPEED_LABEL = "Melt Speed";
        const char* COOL_SPEED_LABEL = "Cool Speed";
        const char* CUT_LENGTH_LABEL = "Cut Length";

        const int RUN_LABLE_INDEX = 0;
        const int RUN_COUNT_LABLE_INDEX = 1;
        const int MELT_LABLE_INDEX = 2;
        const int COOL_LABLE_INDEX = 3;
        const int CUT_LABLE_INDEX = 4;

        String rowLabels[LABEL_COUNT];

        String rowValues[LABEL_COUNT];

        int rowPrecision[LABEL_COUNT];

        int currentColumnIndex;
        //int currentRowIndex;
        int currentLabelIndex;

        PersistantData data;
        EeWrapper eeWrapper;

        SetupDisplay();

        void ZeroOut(LiquidCrystal& lcd);

        void UpdateDisplayAllRows(
            LiquidCrystal& lcd,
            bool& exitSetUpMode);

        void UpdateDisplay(
            LiquidCrystal& lcd, 
            ButtonDirection ButtonDirection,
            bool& exitSetUpMode);

        void ChangeRowValue(
            int rowIndex,
            ButtonDirection direction,
            bool& exitSetUpMode);
};

#endif