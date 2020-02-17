#include "EeWrapper.h"
#include "EEPROM.h"

EeWrapper::EeWrapper() {

}

PersistantData EeWrapper::Get()
{ 
    EEPROM.get(EEPROM_INDEX, data);
    return data;
}

void EeWrapper::Write()
{
    EEPROM.put(EEPROM_INDEX, data);
}