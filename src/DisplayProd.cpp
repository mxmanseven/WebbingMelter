#include "DisplayProd.h"

DisplayProd::DisplayProd()
{
    expectedRunCount = 0;
    currentRunCount = 0;
}

void DisplayProd::ShowRunCount(
            LiquidCrystal& lcd)
{
    lcd.setCursor(0,0);
    char buff[20];
    snprintf(buff, 20, 
        "RUN %d / %d",
        currentRunCount,
        expectedRunCount);

    lcd.print(buff);    
}

void DisplayProd::SetCommand(
            LiquidCrystal& lcd,
            const char* commandStr)
{    
    lcd.clear();

    lcd.setCursor(0,1);
    char buff[21];
    snprintf(buff, 21, "%s              ", commandStr);
    lcd.print(buff);

    
    lcd.setCursor(0, 2);
    lcd.print("                    ");
    lcd.setCursor(0, 3);
    lcd.print("                    ");

    ShowRunCount(lcd);
}