#include "DisplayProd.h"

DisplayProd::DisplayProd()
{
    expectedRunCount = 0;
    currentRunCount = 0;
}

void DisplayProd::ShowCommandAndRunCount(
            LiquidCrystal& lcd,
            char* commandStr)
{    
    lcd.setCursor(0,0);
    char buff[21];
    snprintf(buff, 21, "%s", commandStr);
    lcd.print(buff);
    
    lcd.setCursor(0,1);
    snprintf(buff, 21, 
        "Run %d / %d",
        currentRunCount,
        expectedRunCount);

    lcd.print(buff);    
}

void DisplayProd::SetCommand(
            LiquidCrystal& lcd,
            char* commandStr)
{    
    lcd.setCursor(0,0);
    char buff[21];
    snprintf(buff, 21, "%s", commandStr);
    lcd.print(buff);
}