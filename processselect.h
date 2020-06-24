#ifndef PROCESSSELECT_H
#define PROCESSSELECT_H
#include<QString>
#include<QDebug>
#include"rangelayout.h"
#include"hook.h"
#include<vector>
#include"memregion.h"
#include"lookup.h"
#include<QTableWidget>
#include<QHeaderView>
#include"dataconversion.h"

class ProcessSelect
{
private:
    static ProcessSelect instance;
    ProcessSelect();

    QString gameTitle;
    std::vector<unsigned long long> ranges;
    std::vector<RangeLayout> rangeMap;
    std::vector<MemRegion> memRegions;
    std::vector<QStringList> info;
    std::vector<MemRegion>* processMemRegions;
    Hook* hook;
    QStringList gameInfoHeader;
    QStringList gameInfo;
    bool BE;

    void initializePJ64();
    void initializeDolphin();
    void initializeCemu();
    void initializeGBA();
    void initializeGB();
    void initializeGBC();
    void initializeDesmume();
    void initializeCitra();
    void initializeMM();
    void initializeGG();
    void initializeMD();
    void initializeCD();
    void initializeYabause();
    void initializeNullDC();
    void initializeEPSXE();
    void initializePCSX2();
    void initializeRPCS3();
    void initializePPSSPP();
    void initializePC();

public:
    ProcessSelect(const ProcessSelect&) = delete;
    static ProcessSelect& getInstance();
    std::vector<RangeLayout> getRangeMap();
    void initialize(Hook& hook, std::vector<MemRegion>& processMemRegions, int systemType);
    void setBE(bool val);
    bool isBE();
    QStringList* getInfoHeader();
    QStringList* getInfoData();
};

#endif // PROCESSSELECT_H
