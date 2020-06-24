#ifndef BYTEORDER_H
#define BYTEORDER_H
#include<string>
#include <stdlib.h>

namespace DataConversion
{
    enum ValueType{int8 = 0, uint8 = 1, int16 = 2, uint16 = 3, int32 = 4, uint32 = 5, int64 = 6, uint64 = 7, float_ = 8, double_ = 9, longDouble_ = 10, bool_ = 11, arrInt8 = 12, arrInt16 = 13, arrInt32 = 14, arrInt64 = 15, arrFloat = 16, arrDouble = 17 };

    template<typename dType> dType static text2dType(std::string& txt, int radix, int type)
    {
        dType val;
        switch(type)
        {
        case int8: val = static_cast<char>(std::stoi(txt, nullptr, radix));
        case uint8: val = static_cast<unsigned char>(std::stoul(txt, nullptr, radix));
        case int16: val = static_cast<short>(std::stoi(txt, nullptr, radix));
        case uint16: val = static_cast<unsigned short>(std::stoul(txt, nullptr, radix));
            //24
        case int32: val = static_cast<int>(std::stoi(txt, nullptr, radix));
        case uint32: val = static_cast<unsigned int>(std::stoul(txt, nullptr, radix));
        case int64: val = static_cast<long long>(std::stoll(txt, nullptr, radix));
        case uint64: val = static_cast<unsigned long long>(std::stoull(txt, nullptr, radix));
        //96
            //128
        case float_: val = static_cast<float>(std::stof(txt, nullptr));
        case double_: val = static_cast<float>(std::stod(txt, nullptr));
        case longDouble_: val = static_cast<float>(std::stold(txt, nullptr));
        //case bool_: val = static_cast<float>(std::at(txt, nullptr));
        }

        return val;
    }

    //used to convert all primitive datatypes to BE
    template<typename dType> static inline dType swapBytes(const dType val)
    {
        dType temp = 0;
        char buf;

        for(int i = 0; i < sizeof(val); ++i)
        {
            buf = *((char*)(&val)+i);
            *(reinterpret_cast<char*>(&temp)+sizeof(val)-(1+i)) = buf;
        }
        return temp;
    }

    //reads floats as their binary representation
    template<typename fType, typename dType> static inline dType floatingPointToBinaryInt(const fType val)
    {
        dType temp = 0;
        char buf;

        for(int i = 0; i < sizeof(val); ++i)
        {
            buf = *((char*)(&val)+i);
            *(reinterpret_cast<char*>(&temp)+i) = buf;
        }
        return temp;
    }

    //interprets any int value as a float binary
    template<typename fType, typename dType> static inline fType binaryIntToFloatingPoint(const dType val)
    {
        dType temp = val;
        return *reinterpret_cast<fType*>(&temp);
    }
}
#endif // BYTEORDER_H
