#ifndef MEMREGION_H
#define MEMREGION_H
#include<windows.h>
#include<QString>

//carries information about a processe's memory region
class MemRegion
{
private:
    unsigned long long address;
    unsigned long long size;
    QString protectionStr;
    unsigned int protectionVal;
    bool mapped;
    int mapState;
    unsigned int memMapVal;
    int memType;

public:
    MemRegion(unsigned long long address, unsigned long long size, unsigned int protection, int state, int type)
    {
        this->address = address;
        this->size = size;
        this->mapped = !(state==MEM_FREE);
        this->mapState = state;
        this->memMapVal = protection;
        this->memType = type;
        set_protection(protection);
    }

    MemRegion(unsigned long long address, unsigned long long size)
    {
        this->address = address;
        this->size = size;
    }

    unsigned long long get_address(){ return this->address; }
    unsigned long long get_size(){ return this->size; }
    QString get_protectionStr(){ return this->protectionStr; }
    unsigned int get_protectionVal(){ return this->protectionVal; }
    bool is_mapped(){ return this->mapped; }
    int getMapState(){ return this->mapState; }
    //int getMemMapType(){ return this->memMapVal; }
    int getMemType(){ return this->memType; }

    void set_address(unsigned long long address){ this->address = address; }
    void set_Size(unsigned long long size){ this->size = size; }
    void set_mapped(bool mapped){ this->mapped = mapped; }
    void set_protection(unsigned int protection)
    {
        this->protectionVal = protection;
        switch(protection)
        {
            case PAGE_EXECUTE: this->protectionStr = "Execute"; break;
            case PAGE_READWRITE: this->protectionStr = "Read, Write"; break;
            case PAGE_EXECUTE_READ: this->protectionStr = "Execute, Read"; break;
            case PAGE_EXECUTE_READWRITE: this->protectionStr = "Execute, Read, Write"; break;
            case PAGE_EXECUTE_WRITECOPY: this->protectionStr = "Execute, Write copy"; break;
            case PAGE_NOACCESS: this->protectionStr = "No Access"; break;
            case PAGE_READONLY: this->protectionStr = "Read only"; break;
            case PAGE_WRITECOPY: this->protectionStr = "Write copy"; break;
            case PAGE_GUARD: this->protectionStr = "Guard"; break;
            case PAGE_NOCACHE: this->protectionStr = "Not Cached"; break;
            case PAGE_WRITECOMBINE: this->protectionStr = "Write Combine"; break;
            case PAGESETUPDLGORD: this->protectionStr = "Write Combine"; break;
            case PAGESETUPDLGORDMOTIF: this->protectionStr = "Write Combine"; break;
            case 0: this->protectionStr = "-"; break;
            default: this->protectionStr = "Excuse me wtf?! " + QString::number(protection, 16); break;
        }
    }
};


#endif // MEMREGION_H
