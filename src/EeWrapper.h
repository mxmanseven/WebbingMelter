#ifndef EEWRAPPER_H_
#define EEWRAPPER_H_

struct PersistantData
{
    int runCount;
    float meltSpeed;
    float coolSpeed;
    float cutLength;
};

class EeWrapper
{
    public:
    PersistantData data;
    const int EEPROM_INDEX = 0;

    EeWrapper();

    PersistantData Get();
    void Write();
};

#endif