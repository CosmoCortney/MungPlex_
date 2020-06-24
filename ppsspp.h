#ifndef PPSSPP_H
#define PPSSPP_H
#include<QString>
#include<QDebug>
#include"rangelayout.h"
#include"hook.h"
#include<vector>
#include"memregion.h"

class PPSSPP
{
private:
    QString gameTitle;
    std::vector<unsigned long long> ranges;
    std::vector<RangeLayout> rangeMap;
    std::vector<MemRegion> memRegions;
    std::vector<QStringList> info;

public:
    PPSSPP(){}

    std::vector<RangeLayout> get_rangeMap(){ return this->rangeMap; }

    //gets logical addresses of all emulated memory regions
    void initialize(Hook& hook, std::vector<MemRegion>& memArr)
    {
        unsigned long long ramAddress = 0;
        memRegions.clear();
        for(unsigned long long i = 0; i < memArr.size(); ++i)
        {
            unsigned long long msize = memArr.at(i).get_size();
            if(msize == 0x1f00000)
            {
                ramAddress = memArr.at(i).get_address();
                memRegions.push_back(MemRegion(ramAddress+0x800000, 0x1800000));
                rangeMap.push_back(RangeLayout(ramAddress+0x800000, 0x00000000, 0x1800000, "Main Memory"));
                return;
            }
        }
    }
};
#endif // PPSSPP_H
