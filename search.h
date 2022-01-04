#ifndef SEARCH_H
#define SEARCH_H

#include <QString>
#include <QObject>
#include "hook.h"
#include"rangelayout.h"
#include <vector>
#include"memregion.h"
#include<QDebug>
#include<windows.h>
#include<QFile>
#include<QDataStream>
#include"dataconversion.h"
#include<functional>
#include"settings.h"
#include"operativearray.h"

//the functor classes 'functor' and 'readOrder' are being used to use functions as opjects. This is needed to prevent unnecessary comparision within memory searches



//defines all search functions
//todo: make the search being cancelled if a bitwise operator is attempted to be used with a float or double (concepts?). So they can be used for integer types
template<typename dType> class functor
{
public:
    std::function<bool(dType&, dType&)> op;

    inline bool operator()(dType x, dType y) const { return this->op(x, y); }

    static inline bool eq(dType x, dType y) { return x == y; }
    static inline bool ne(dType x, dType y) { return x != y; }
    static inline bool lt(dType x, dType y) { return x <  y; }
    static inline bool le(dType x, dType y) { return x <= y; }
    static inline bool gt(dType x, dType y) { return x >  y; }
    static inline bool ge(dType x, dType y) { return x >= y; }
    //static inline bool AND(dType x, dType y) { return (x & y) == y; } //x contains all bits of y
    //static inline bool OR(dType x, dType y) { return (x & y) != 0; } //x contains all bits of y
};

//defines the type of read order to be used
template<typename dType, typename PTR> class readOrder
{
public:
    std::function<dType(PTR)> load;

    inline dType operator()(PTR ptr) const { return this->load(ptr); }

    static inline dType loadLE(PTR ptr) { return *ptr; }
    static inline dType loadBE(PTR ptr) { return DataConversion::swapBytes<dType>(*ptr); }
};

class Search
{
private:
    Hook* hook;
    enum dataType{ uInt8 = 0, sInt8 = 1, uInt16 = 2, sInt16 = 3, uInt24 = 4, sInt24 = 5, uInt32 = 6, sInt32 = 7, uint64 = 8, sInt64 = 9, uInt96 = 10, uInt128 = 11, _float = 12, _double = 13, textASCII = 14, textUTF16 = 15, textUTF8 = 16, byteArray = 17, colRGBint = 18, colRGBAint = 19, colRGBfloat = 20, colRGBAfloat = 21 };
    enum procSelect{PC_ = 0, Mesen = 1, ZSNES9x = 2, VBjin = 3, Project64 = 4, Dolphin_ = 5, Cemu = 6, Yuzu = 7, VGBA = 8, DesMume = 9, Citra = 10};
    unsigned long long alignmentSize;
    unsigned long long VsearchStart;
    unsigned long long VsearchEnd;
    unsigned long long searchStart;
    unsigned long long searchEnd;
    unsigned long long searchSize;
    int iterations = 0;
    unsigned long long resultCount = 0;
    long long rangeSize;
    unsigned long long* oldOffsetListPTR;
    unsigned long long* newOffsetListPTR;
    bool firstSearch = true;
    unsigned char* newMemDumpPTR;
    unsigned char* oldMemDumpPTR;
    long long dumpSize = 0;
    unsigned long long offsetListSize = 0;
    unsigned long long hits = 0;
    int condition = 0;
    unsigned long long knownValue;
    bool isKnown = false;
    bool isAligned = false;

    //functor instance
    template<typename dType> functor<dType>* getComparisionType()
    {
        functor<dType>* fx = new functor<dType>();

        switch(condition)
        {
        case 0: fx->op = functor<dType>::eq; break;
        case 1: fx->op = functor<dType>::ne; break;
        case 2: fx->op = functor<dType>::lt; break;
        case 3: fx->op = functor<dType>::le; break;
        case 4: fx->op = functor<dType>::gt; break;
        case 5: fx->op = functor<dType>::ge; break;
        //case 6: fx->op = functor<dType>::AND; break;
        //case 7: fx->op = functor<dType>::OR; break;
        }
        return fx;
    }

    //functor instance
    template<typename dType, typename PTR> readOrder<dType, PTR>* getReadOrder(bool BE)
    {
        readOrder<dType, PTR>* read = new readOrder<dType, PTR>();
        if(BE) { read->load = readOrder<dType, PTR>::loadBE; }
        else   { read->load = readOrder<dType, PTR>::loadLE; }
        return read;
    }

public:
    Search(){}
    ~Search(){}
    Search(Hook* hook);
    void set_hook(Hook* hook){ this->hook = hook; }
    void set_resultCount(unsigned long long amount){ this->resultCount = amount; }
    void set_offsetListAddr(unsigned long long* addr){ this->newOffsetListPTR = addr; }
    void incIteration(){this->iterations++;}
    void writeFileToDisk(char* data, QIODevice* file, long long size);
    void readFileFromDisk(QIODevice* file, char* map, long long size);
    void freeDumps() { delete newOffsetListPTR; delete newMemDumpPTR; delete oldMemDumpPTR; delete oldOffsetListPTR; }
    void set_condition(int x){ this->condition = x; }
    void set_alignment(unsigned long long val) { this->alignmentSize = val; }
    void set_knownValue(unsigned long long val){ this->knownValue = val; }
    void set_known(bool x){ this->isKnown = x; }
    void cancel();
    int getIterations(){ return this->iterations; }

    //limits the range of where to search for values
    void setSearchRange(unsigned long long Vstart, unsigned long long Vend, unsigned long long start)
    {
        this->VsearchStart = Vstart;
        this->VsearchEnd = Vend;
        this->searchStart = start;
        this->searchSize = Vend - Vstart + 1;
        this->searchEnd = start + this->searchSize;
    }

    //pokes a value into memory
    template<typename dType>void poke(char* address, dType val)
    {
        LPVOID targetAddress = address - VsearchStart + searchStart;
        WriteProcessMemory(hook->getHandle(), targetAddress, &val, sizeof(val), 0);
    }

    unsigned long long get_amountOfRanges();
    void* dump(void* sourceAddress, unsigned long long size);
    unsigned long long get_resultCount(){ return this->resultCount; }
    unsigned long long* get_offsetList(){ return this->newOffsetListPTR; }
    unsigned char* get_dumpAddress() { return this->newMemDumpPTR; }
    unsigned char* get_oldDumpAddress() { return this->oldMemDumpPTR; }
    unsigned long long get_virtualStartAddress(){ return this->VsearchStart; }
    bool isFirstSearch(){ return this->firstSearch; }
    void firstSearchDone(bool val){ this->firstSearch = val; }
    void undoFirstSearch(){ this->firstSearch = true; }
    void deallocDump(){ free(this->newMemDumpPTR); }
    unsigned long long get_hitsCount(){ return this->hits; }

    void newRamDump()
    { ReadProcessMemory(hook->getHandle(), reinterpret_cast<LPCVOID>(searchStart), newMemDumpPTR, searchSize, nullptr); }

    /* performs searches
     * comparision type and read order (BE/LE) are  defined as functors befor each search process begins. This prevents checks which comparision type is being used for each value comparision
     * after the initial scan the pointer of the current memory dump (newMemDump) is copied to oldMemDump. newMemDump will then be cleared and use for the actual new memory dump
     * todo: support for bitwise comparisions. cancel search when this is attempted to be done with floats or doubles (concepts?).
     * todo: support for AnyInt (int24, int96, int 128), byte array, string (ASCII, utf-8, utf-16, utf-32, various ANSI types), colors (int RGB, int RGBA, float RGB, float RGBA)
     */
    template<typename dType> void search(bool isKnown, bool BE, dType Tval)
    {
        functor<dType>* fx = getComparisionType<dType>();//defines functors here
        readOrder<dType, dType*>* read = getReadOrder<dType, dType*>(BE);

        if(firstSearch)
        {
            firstSearch = false;
            hits = 0;

            oldMemDumpPTR = reinterpret_cast<unsigned char*>(malloc(searchSize));
            newMemDumpPTR = reinterpret_cast<unsigned char*>(malloc(searchSize));
            oldOffsetListPTR = reinterpret_cast<unsigned long long*>(malloc(searchSize / alignmentSize * 8));
            newOffsetListPTR = reinterpret_cast<unsigned long long*>(malloc(searchSize / alignmentSize * 8));
            newRamDump();

            if(isKnown) { compare_known_initial<dType*, dType>(reinterpret_cast<dType*>(newMemDumpPTR), Tval, fx, read); }
            else        { searchUnkownInitial<dType>(); }
        }
        else
        {
            delete oldMemDumpPTR;
            delete oldOffsetListPTR;
            oldMemDumpPTR = newMemDumpPTR;
            oldOffsetListPTR = newOffsetListPTR;
            newMemDumpPTR = reinterpret_cast<unsigned char*>(malloc(searchSize));
            newOffsetListPTR = reinterpret_cast<unsigned long long*>(malloc(hits * 8));
            newRamDump();
            offsetListSize = hits * 8;
            hits = 0;

            if(isKnown) { compare_known<dType*, dType>(reinterpret_cast<dType*>(newMemDumpPTR), Tval, fx, read); }
            else        { compare_unk<dType*, dType>(reinterpret_cast<dType*>(oldMemDumpPTR), reinterpret_cast<dType*>(newMemDumpPTR), fx, read); }
        }

        delete fx;
        delete read;
    }

    //conpares FIRST known values concerning to the fx functor
    template<typename PTR, typename dType> void compare_known_initial(PTR newMemDumpPTR, dType val, functor<dType>* fx, readOrder<dType, PTR>* read)
    {
        unsigned long long offsetChunkSize = searchSize * 8 / alignmentSize;
        newOffsetListPTR = reinterpret_cast<unsigned long long*>(malloc(offsetChunkSize));
        dType newVal;

        for(unsigned long long i = 0; i < offsetChunkSize/(8/alignmentSize); i += alignmentSize)
        {
            newVal = read->operator()(reinterpret_cast<PTR>(reinterpret_cast<char*>(newMemDumpPTR) + i));
            if(fx->operator()(newVal, val))
            {
                *(newOffsetListPTR+hits) = i;
                hits++;
            }
        }
    }

    //conpares known values concerning to the fx functor
    template<typename PTR, typename dType> void compare_known(PTR newMemDumpPTR, dType val, functor<dType>* fx, readOrder<dType, PTR>* read)
    {
        dType newVal;

        for(unsigned long long i = 0; i < offsetListSize/8; ++i)
        {
            newVal = read->operator()(reinterpret_cast<PTR>(reinterpret_cast<char*>(newMemDumpPTR) + *(oldOffsetListPTR+i)));
            if(fx->operator()(newVal, val))
            {
                *(newOffsetListPTR+hits) = *(oldOffsetListPTR+i);
                hits++;
            }
        }
    }

    //conpares FIRST unknown values concerning to the fx functor
    template<typename dType> void searchUnkownInitial()
    {
        unsigned long long offsetChunkSize = searchSize * 8 / alignmentSize;
        newOffsetListPTR = reinterpret_cast<unsigned long long*>(malloc(offsetChunkSize));

        for(struct{unsigned long long offset = 0; unsigned long long count = 0;} i; i.offset < searchSize; i.offset += alignmentSize, ++i.count)
        {
            *(newOffsetListPTR + i.count) = i.offset;
        }

        hits = searchSize / alignmentSize;
    }

    //conpares unknown values concerning to the fx functor
    template<typename PTR, typename dType> void compare_unk(PTR oldMemDumpPTR, PTR newMemDumpPTR, functor<dType>* fx, readOrder<dType, PTR>* read)
    {
        dType oldVal, newVal;

        for(unsigned long long i = 0; i < (offsetListSize/8); ++i)
        {
            newVal = read->operator()(reinterpret_cast<PTR>(reinterpret_cast<char*>(newMemDumpPTR) + *(oldOffsetListPTR+i)));
            oldVal = read->operator()(reinterpret_cast<PTR>(reinterpret_cast<char*>(oldMemDumpPTR) + *(oldOffsetListPTR+i)));
            if(fx->operator()(newVal, oldVal))
            {
                *(newOffsetListPTR+hits) = *(oldOffsetListPTR+i);
                ++hits;
            }
        }
    }

    /* //for testing
    template<typename PTR, typename dType, typename tType> void compare_known_float_initial(PTR newMemDumpPTR, dType val, bool BE, functor<dType>* fx)
    {
        dType newVal;
        tType tempVal;

        switch(condition)
        {
        case 0:
            for(int i = 0; i < (offsetListSize/sizeof(long long)); ++i)
            {
                if(BE)
                {
                    tempVal = DataConversion::swapBytes<tType>(*(tType*)(((char*)newMemDumpPTR) + i));
                    newVal = DataConversion::binaryIntToFloatingPoint<dType, tType>(tempVal);
                }
                else{ newVal = *(PTR)(((char*)newMemDumpPTR) + i); }

                if(newVal == val)
                {
                    *(newOffsetListPTR+hits) = i;
                    hits++;
                }
            }
            break;
        }
    }*/
};

#endif // SEARCH_H
