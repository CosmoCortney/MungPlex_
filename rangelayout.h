#ifndef RANGELAYOUT_H
#define RANGELAYOUT_H
#include<QString>
#include<vector>

//carries information about an emulated memory range
class RangeLayout
{
private:
    unsigned long long address;         //address of emulated ram in emu
    unsigned long long virtualAddress;
    unsigned long long size;
    QString* label;

public:
    RangeLayout();
    RangeLayout(unsigned long long address, unsigned long long virtualAddress, QString label)
    {
        this->address = address;
        this->virtualAddress = virtualAddress;
        this->label = new QString(label);
    }

    RangeLayout(unsigned long long address, unsigned long long virtualAddress, unsigned long long size)
    {
        this->address = address;
        this->virtualAddress = virtualAddress;
        this->size = size;
    }

    RangeLayout(unsigned long long address, unsigned long long virtualAddress, unsigned long long size, QString label)
    {
        this->address = address;
        this->virtualAddress = virtualAddress;
        this->size = size;
        this->label = new QString(label);
    }

    void set_size(unsigned long long size){ this->size = size; }

    unsigned long long get_address(){ return this->address; }   //returns address of emulated memory [emulator]
    unsigned long long get_virtualAddress(){ return this->virtualAddress; } //returns address of memory the game computes with [game]
    QString* get_lable() { return this->label; }
    unsigned long long get_size(){ return this->size; }
};

#endif // RANGELAYOUT_H
