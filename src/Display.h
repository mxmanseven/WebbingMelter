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

        const char* RUN_LABEL = "RUN";
        const char* LIFETIME_RUN_LABEL = "RUN-TOTAL";
        const char* RUN_COUNT_LABEL =  "CUT QTY";
        const char* MELT_DIST_LABEL = "MELT DIST";
        const char* CUT_LENGTH_LABEL = "CUT LENGTH";

        const int RUN_LABLE_INDEX = 0;
        const int LIFETIME_RUN_LABLE_INDEX = 1;
        const int RUN_COUNT_LABLE_INDEX = 2;
        const int MELT_LABLE_INDEX = 3;
        const int CUT_LABLE_INDEX = 4;

        String rowLabels[LABEL_COUNT];
        String rowValues[LABEL_COUNT];
        int rowPrecision[LABEL_COUNT];

        int currentColumnIndex;
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

        void WritePersistantDate();
};

#endif