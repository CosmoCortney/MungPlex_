#ifndef POINTERSEARCH_H
#define POINTERSEARCH_H
#include<QString>
#include<QDir>
#include<QFile>
#include<QDebug>
#include<QMessageBox>
#include<vector>
#include<windows.h>
#include"hook.h"
#include"rangelayout.h"
#include"dataconversion.h"


class PointerSearch
{
private:
    std::vector<RangeLayout> ranges;
    unsigned long long selectedRange;
    char* newRange = nullptr;
    Hook hook;
    QFile* dumpFile;
    bool BE = false;
    bool isAligned = true;
    bool isFirstSearch = true;
    bool is64bitWidth = false;
    unsigned long long resultCount;
    unsigned long long* results = nullptr;
    int* offsets = nullptr;
    int* offsets2 = nullptr;
    unsigned long long baseAddress;
    unsigned long long pointerDestination;
    unsigned long long maxOffset;
    unsigned long long minOffset;
    int level = 1;

public:
    PointerSearch();
    void setPointerLevel(int val) { this->level = val; }
    void setRangeLayout(std::vector<RangeLayout>& ranges){ this->ranges = ranges; }
    void setSelectedRange(unsigned long long range){ this->selectedRange = range; }
    void setHook(Hook& hook){ this->hook = hook; }
    bool dumpRangeToFile(char* data, QIODevice* file, long long size);
    bool readRange();
    bool dump();
    bool load();
    void setPath(QString& dumpFile){ this->dumpFile = new QFile(dumpFile); }
    bool loadRangeFromDisk(QIODevice* file, long long size);
    char* getDumpAddress(){ return newRange; }
    void setBE(bool val){ BE = val; }
    void setAligned(bool val){ isAligned = val; }
    void dumpToBE();
    void set64bitWidth(bool val){ is64bitWidth = val; }
    unsigned long long search();
    unsigned long long* getResultsAddress(){ return results; }
    int* getOffsetList(){ return offsets; }
    void setSearchInfo(unsigned long long baseAddr, unsigned long long maxOffs, unsigned long long minOffs, unsigned long long destination)
    { baseAddress = baseAddr; maxOffset = maxOffs; minOffset = minOffs; pointerDestination = destination; }
};

#endif // POINTERSEARCH_H
