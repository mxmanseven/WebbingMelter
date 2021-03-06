#ifndef DISPLAY_PROD_H_
#define DISPLAY_PROD_H_

#include <Wire.h>
#include <LiquidCrystal.h>

class DisplayProd
{
    public:
        int expectedRunCount;
        int currentRunCount;
        
        DisplayProd();

        void ShowRunCount(
            LiquidCrystal& lcd);

        void SetCommand(
            LiquidCrystal& lcd,
            const char* commandStr);
};

#endif