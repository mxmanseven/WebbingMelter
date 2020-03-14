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

        void ShowCommandAndRunCount(
            LiquidCrystal& lcd,
            char* commandStr);

        void SetCommand(
            LiquidCrystal& lcd,
            char* commandStr);
};

#endif