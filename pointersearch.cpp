#include "pointersearch.h"

PointerSearch::PointerSearch()
{

}

//reads selected memory range
bool PointerSearch::readRange()
{
    free(newRange);
    newRange = reinterpret_cast<char*>(malloc(ranges[selectedRange].get_size()));
    return ReadProcessMemory(hook.getHandle(), reinterpret_cast<LPVOID>(ranges[selectedRange].get_address()), newRange, ranges[selectedRange].get_size(), nullptr);
}

//selects range for dumping
bool PointerSearch::dump()
{
    return dumpRangeToFile((char*)newRange, dumpFile, ranges[selectedRange].get_size());
}


bool PointerSearch::load()
{
    if(!isFirstSearch) { free(newRange); }
    return loadRangeFromDisk(dumpFile, dumpFile->size());
}

//reads mem dump from disk. todo: allow files > 2GB
bool PointerSearch::loadRangeFromDisk(QIODevice *file, long long size)
{
    newRange = reinterpret_cast<char*>(malloc(size));
    if(!file->open(QIODevice::ReadOnly)) { qDebug() << "read error"; }
    QDataStream readFile(file);
    readFile.setByteOrder(QDataStream::LittleEndian);
    readFile.readRawData(newRange, size);
    file->close();
    return true;
}

//converts dump to BE
void PointerSearch::dumpToBE()
{
    long long size = dumpFile->size();
    int* ptr = reinterpret_cast<int*>(newRange);

    for(int offset = 0; offset < size/4; ++offset)
    {
        *(ptr+offset) = DataConversion::swapBytes<int>(*(ptr+offset));
    }
}

//searches for pointers
unsigned long long PointerSearch::search()
{
    unsigned long long size = dumpFile->size();

    if(is64bitWidth)
    {

    }
    else    //address width is 32 bits
    {
        if(isAligned)
        {
            size /= 4;
            if(BE){ dumpToBE(); }

            //qDebug() << dumpFile->fileName();

            switch(level)
            {
            case 1:
                if(isFirstSearch)
                {
                    resultCount = 0;
                    unsigned int* ptr = reinterpret_cast<unsigned int*>(newRange);
                    unsigned long long temp = 0;
                    results = reinterpret_cast<unsigned long long*>(malloc(size/4));
                    offsets = reinterpret_cast<int*>(malloc(size/4));
                    for(unsigned long long offset = 0; offset < size; ++offset)
                    {
                        temp = *(ptr + offset);
                        if(temp != 0 && temp != 0x3F800000 && ((temp & 3) == 0))
                        {
                            if((temp + maxOffset) >= pointerDestination && (temp - minOffset) <= pointerDestination)
                            {
                                *(results + resultCount) = offset*4;
                                *(offsets + resultCount) = (int)pointerDestination - (int)temp;
                                ++resultCount;
                            }
                        }
                    }
                    qDebug() << results;
                    isFirstSearch = false;
                    qDebug() << offsets;
                }
                else    //next iteration
                {
                    unsigned long long newResultCount = 0;
                    unsigned int* ptr = reinterpret_cast<unsigned int*>(newRange);

                    for(unsigned long long offset = 0; offset < resultCount; ++offset)
                    {
                        unsigned int nthResult = *(results + offset);
                        unsigned int nthOffset = *(offsets + offset);
                        if(*(ptr + nthResult/4) + nthOffset == pointerDestination)
                        {
                            *(results + newResultCount) = nthResult;
                            *(offsets + newResultCount) = nthOffset;
                            ++newResultCount;
                        }
                    }
                    resultCount = newResultCount;
                }
            break;
            case 2:
                if(isFirstSearch)
                {
                    resultCount = 0;
                    unsigned int* ptr = reinterpret_cast<unsigned int*>(newRange);
                    unsigned long long temp = 0;
                    unsigned long long temp2 = 0;
                    results = reinterpret_cast<unsigned long long*>(malloc(size/4));
                    offsets = reinterpret_cast<int*>(malloc(size/4));

                    for(unsigned long long offset = 0; offset < size; ++offset)
                    {
                        temp = *(ptr + offset);
                        if(temp != 0 && temp != 0x3F800000 && ((temp & 3) == 0) && temp >= baseAddress && temp <= (baseAddress+size*4))
                        {
                            unsigned int* ptr2 = nullptr;
                            unsigned int start = temp - minOffset;
                            unsigned int end = temp + maxOffset;

                            for(unsigned int count = start; count <= end && count < (baseAddress+size*4) && count >= baseAddress; count+=4)//unsigned long long offset2 = 0; offset2 < size; ++offset2
                            {
                                ptr2 = reinterpret_cast<unsigned int*>(newRange + (count-baseAddress));
                                temp2 = *ptr2;

                                if((temp2 + maxOffset) >= pointerDestination && (temp2 - minOffset) <= pointerDestination && ((temp2 & 3) == 0)) //if((temp + maxOffset) >= pointerDestination && (temp - minOffset) <= pointerDestination)
                                {
                                    *(results + resultCount) = offset*4;
                                    *(offsets + resultCount*2) = (int)count - (int)temp;
                                    *(offsets + resultCount*2+1) = (int)pointerDestination - (int)temp2;
                                    ++resultCount;
                                }
                            }
                        }
                    }
                    //qDebug() << results;
                    isFirstSearch = false;
                    //qDebug() << offsets;
                }
                else    //next iteration
                {
                    unsigned long long newResultCount = 0;
                    unsigned int* ptr = reinterpret_cast<unsigned int*>(newRange);

                    for(unsigned long long offset = 0; offset < resultCount; ++offset)
                    {
                        unsigned int nthResult = *(results + offset);
                        unsigned int nthOffset = *(offsets + offset*2);
                        unsigned int nthOffset2 = *(offsets + offset*2+1);
                        unsigned int checkMe = *(ptr + nthResult/4) + nthOffset;//contains level-1 pointer

                        if(checkMe >= baseAddress && checkMe < (baseAddress+size*4))
                        {
                             checkMe -= baseAddress;
                             checkMe = *(ptr + checkMe/4) + nthOffset2;

                             if(checkMe == pointerDestination)
                             {
                                 *(results + newResultCount) = nthResult;
                                 *(offsets + newResultCount*2) = nthOffset;
                                 *(offsets + newResultCount*2+1) = nthOffset2;
                                 ++newResultCount;
                             }
                        }
                    }
                    resultCount = newResultCount;
                }
            break;
            }
        }
    }
    return resultCount;
}

//writes dump to disk
bool PointerSearch::dumpRangeToFile(char* data, QIODevice* file, long long size)
{
    if(!file->open(QIODevice::WriteOnly))
    { qDebug() << "Error: cannot write file"; }

    QDataStream writeFile(file);
    writeFile.setByteOrder(QDataStream::LittleEndian);
    writeFile.setVersion(QDataStream::Qt_5_12);
    qDebug() << QString::number(writeFile.writeRawData(data, size), 16);
    file->close();
    return true;
}


