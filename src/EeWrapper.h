#ifndef EEWRAPPER_H_
#define EEWRAPPER_H_

struct PersistantData
{
    int runCount;
    int lifeTimerunCount;
    float meltDist;
    float coolSpeed;
    float cutLength;
};

class EeWrapper
{
    public:
    //PersistantData data;
    const int EEPROM_INDEX = 0;

    EeWrapper();

    void Get(PersistantData& data);
    void Write(PersistantData& data);
    void Test();
};

#endif