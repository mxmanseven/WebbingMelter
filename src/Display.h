#ifndef DISPLAY_H_
#define DISPLAY_H_

#include <Wire.h>
#include <LiquidCrystal.h>

enum ButtonDirection {
    None,
    Left,
    Right,
    Up,
    Down
};

class Display
{
    public:
        const int LCD_COLUMN_COUNT = 20;
        const int LCD_ROW_COUNT = 4;
        static const int LABEL_COUNT = 5;

        const char* RUN_LABEL = "Run";
        const char* RUN_COUNT_LABEL = "Run Count";
        const char* MELT_SPEED_LABEL = "Melt Speed";
        const char* COOL_SPEED_LABEL = "Cool Speed";
        const char* CUT_LENGTH_LABEL = "Cut Length";

        String rowLabels[LABEL_COUNT];

        int currentColumnIndex;
        int currentRowIndex;
        int currentLabelIndex;

        Display();
        void UpdateDisplayAllRows(LiquidCrystal& lcd);
        void UpdateDisplay(
            LiquidCrystal& lcd, 
            ButtonDirection ButtonDirection);
};

#endif