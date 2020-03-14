#include "EeWrapper.h"
#include "EEPROM.h"

EeWrapper::EeWrapper() {
}

void EeWrapper::Get(PersistantData& data)
{ 
    EEPROM.get(EEPROM_INDEX, data);

    // data.coolSpeed = 12;
    // data.meltSpeed = 1.9;
     //data.runCount = 12;
    // data.cutLength = 4.125;
}

void EeWrapper::Write(PersistantData& data)
{
    EEPROM.put(EEPROM_INDEX, data);
}

void EeWrapper::Test()
{
  PersistantData data;
  Get(data);
  data.coolSpeed = 12;
  data.meltSpeed = 123;
  data.runCount = 12345;
  data.cutLength = 4.125;

  Write(data);

  PersistantData d2;
  Get(d2);
}