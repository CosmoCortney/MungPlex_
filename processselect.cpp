#include "processselect.h"

ProcessSelect::ProcessSelect()
{

}

ProcessSelect& ProcessSelect::getInstance()
{
    static ProcessSelect instance;
    return instance;
}

std::vector<RangeLayout> ProcessSelect::getRangeMap(){ return this->rangeMap; }

void ProcessSelect::setBE(bool val){ this->BE = val; }

bool ProcessSelect::isBE(){ return this->BE; }

QStringList* ProcessSelect::getInfoData(){ return &gameInfo; }

QStringList* ProcessSelect::getInfoHeader(){ return &gameInfoHeader; }

void ProcessSelect::initialize(Hook& hook, std::vector<MemRegion>& processMemRegions, int systemType)
{
    this->processMemRegions = &processMemRegions;
    this->hook = &hook;
    this->memRegions.clear();
    this->gameInfo.clear();
    this->gameInfoHeader.clear();

    switch(systemType)
    {
    case lookup::Project64_: initializePJ64(); break;
    case lookup::Dolphin_: initializeDolphin(); break;
    case lookup::Cemu_: initializeCemu(); break;
    case lookup::VBA_GB:  initializeGB();  break;
    case lookup::VBA_GBC: initializeGBC(); break;
    case lookup::VBA_GBA: initializeGBA(); break;
    case lookup::DesMume_: initializeDesmume(); break;
    case lookup::Citra_: initializeCitra(); break;
    case lookup::FUSION_MM: initializeMM(); break;
    case lookup::FUSION_MD: initializeMD(); break;
    case lookup::FUSION_CD: initializeCD(); break;
    case lookup::FUSION_GG: initializeGG(); break;
    case lookup::Yabause_: initializeYabause(); break;
    case lookup::nulldc_: initializeNullDC(); break;
    case lookup::EPSXE_: initializeEPSXE(); break;
    case lookup::PCSX2_: initializePCSX2(); break;
    case lookup::RPCS3_: initializeRPCS3(); break;
    case lookup::ppsspp_: initializePPSSPP(); break;
     case lookup::PC_: initializePC(); break;
    }
}

void ProcessSelect::initializeDolphin()
{
    BE = true;

    unsigned long long mramAddress = 0, exramAddress = 0;
    unsigned int wiiLayout = 0, gcnLayout = 0, wiiwareID = 0, gameType = 0, FSTBegin, FSTSize, version;
    int discNumber = 0, discVersion = 0, streamingFlag, streamingBufferSize, CPUSpeed, BUSSpeed;
    char first_byte, second_byte, platformFlag, regionFlag, IDBuffer[6], discInfo[4];
    QString gameID, regionStr, makerCode, platform, releaseType;

    memRegions.clear();
    for(unsigned long long i = 0; i < processMemRegions->size(); ++i)
    {
        if(processMemRegions->at(i).get_size() == 0x2000000)
        {
            unsigned int temp;
            ReadProcessMemory(hook->getHandle(), reinterpret_cast<LPCVOID>(processMemRegions->at(i).get_address()+0x28), &temp, 4, nullptr);
            if(temp == 0x8001)
            {
                memRegions.push_back(MemRegion(mramAddress, 0x1800000));
                mramAddress = processMemRegions->at(i).get_address();
                ReadProcessMemory(hook->getHandle(), reinterpret_cast<LPCVOID>(mramAddress+0x18), &wiiLayout, 4, nullptr);
                ReadProcessMemory(hook->getHandle(), reinterpret_cast<LPCVOID>(mramAddress+0x1c), &gcnLayout, 4, nullptr);
                break;
            }
        }
    }

    if(wiiLayout == 0xa39e1c5d || (wiiLayout == 0 && gcnLayout == 0))
    {
        for(unsigned long long i = 0; i < processMemRegions->size(); i++)
        {
            if(processMemRegions->at(i).get_size() == 0x4000000)
            {
                unsigned char temp;
                ReadProcessMemory(hook->getHandle(), reinterpret_cast<LPCVOID>(processMemRegions->at(i).get_address()+0x1), &temp, 1, 0);
                if(temp == 0x9f)
                {
                    memRegions.push_back(MemRegion(exramAddress, 0x4000000));
                    exramAddress = processMemRegions->at(i).get_address();
                    break;
                }
            }
        }
    }

    ReadProcessMemory(hook->getHandle(), reinterpret_cast<LPCVOID>(mramAddress+0x3180), &wiiwareID, 4, nullptr);
    ReadProcessMemory(hook->getHandle(), reinterpret_cast<LPCVOID>(mramAddress), &first_byte, 1, nullptr);              //     infoTable.setItem(2, 0, new QTableWidgetItem(QString::fromStdString(std::string(first_byte << second_byte))));
    ReadProcessMemory(hook->getHandle(), reinterpret_cast<LPCVOID>(mramAddress+1), &second_byte, 1, nullptr);
    rangeMap.clear();


    if(gcnLayout == 0x3d9f33c2)
    {
        qDebug() << "GCN game detected";
        qDebug() << QString::number(gcnLayout, 16);
        gameType = 1;
        rangeMap.push_back(RangeLayout(mramAddress, 0x80000000, 0x1800000, "MEM1"));
    }
    else if(wiiLayout == 0xa39e1c5d)
    {
        qDebug() << "Wii game detected";
        qDebug() << QString::number(wiiLayout, 16);
        gameType = 2;
        rangeMap.push_back(RangeLayout(mramAddress,  0x80000000, 0x1800000, "MEM1"));
        rangeMap.push_back(RangeLayout(exramAddress, 0x90000000, 0x4000000, "MEM2"));
    }
    else if(wiiwareID != 0 && first_byte == 0)
    {
        qDebug() << "WiiWare game detected";
        qDebug() << QString::number(wiiwareID, 16);
        platform = QChar(wiiwareID & 0x000000FF);
        gameType = 3;
        rangeMap.push_back(RangeLayout(mramAddress,  0x80000000, 0x1800000, "MEM1"));
        rangeMap.push_back(RangeLayout(exramAddress, 0x90000000, 0x4000000, "MEM2"));
    }

                                            //0           //1        //2          //3         //4              //5                  //6
    this->gameInfoHeader = QStringList() << "Platform" << "Title" << "Game ID" << "Region" << "Region Code" << "Maker/Publisher" << "Maker/Publisher ID"
                              //7              //8               //9                 //10                       //11                 //12
                           << "Disc Number" << "Disc Version" << "Streaming Flag" << "Streaming Buffer Size" << "CPU Speed (KHz)" << "BUS Speed (KHz)"
                              //13           //14          //15         //16
                           << "FST Begin" << "FST Size" << "Version" << "Release Type";

    ReadProcessMemory(hook->getHandle(), reinterpret_cast<LPCVOID>(mramAddress + (gameType == 3 ? 0x3180 : 0)), &IDBuffer, 6, nullptr);
    gameID = IDBuffer[0];
    for(int i = 1; i < 4; i++){ gameID.append(IDBuffer[i]); }
    regionFlag = IDBuffer[3];
    makerCode = IDBuffer[4];
    makerCode.append(IDBuffer[5]);

    ReadProcessMemory(hook->getHandle(), reinterpret_cast<LPCVOID>(mramAddress+6), &discInfo, 4, nullptr);
    discNumber = static_cast<int>(discInfo[0]);
    discVersion = static_cast<int>(discInfo[1]);
    streamingFlag = static_cast<int>(discInfo[2]);
    streamingBufferSize = static_cast<int>(discInfo[3]);

    switch(gameType)
    {
        case 1: platform = "GameCube"; break;
        case 2: platform = "Wii"; break;
        case 3:
            platform = "Virtual Console: ";
            switch(IDBuffer[0])
            {
                case 'W': platform = "WiiWare"; break;
                case 'N': platform.append("N64"); break;
                case 'A': platform.append("Arcade"); break;
                case 'H': platform = "Wii Channel/Homebrew"; break;
                case 'X': platform = "WiiWare Demo"; break;
                case 'L': platform.append("Sega Master System"); break;
                case 'M': platform.append("Sega Mega Drive"); break;
                case 'E': platform.append("NeoGeo"); break;
                case 'C': platform.append("Commodore"); break;
                case 'Q': platform.append("TurboGrafxCD"); break;
                case 'P': platform.append("TurboGrafx16"); break;
                case 'J': platform.append("SNES"); break;
                case 'F': platform.append("NES"); break;
                default:  platform = "Wii Unknown"; break;
            }
        break;
    }

    this->gameInfo.append(platform);
    this->gameInfo.append(""); //todo: get game title
    this->gameInfo.append(gameID);

    switch(regionFlag)
    {
        case 'A': regionStr = "Region Free"; break;
        case 'C': regionStr = "China"; break;
        case 'D': regionStr = "Germany"; break;
        case 'E': regionStr = "North America"; break;
        case 'F': regionStr = "France"; break;
        case 'I': regionStr = "Italy"; break;
        case 'J': regionStr = "Japan"; break;
        case 'K': regionStr = "South Korea"; break;
        case 'P': regionStr = "Europe"; break;
        case 'R': regionStr = "Russia"; break;
        case 'S': regionStr = "Spain"; break;
        case 'T': regionStr = "Taiwan"; break;
        case 'U': regionStr = "ɐıʃɐɹʇsn∀"; break;
        case 'X': regionStr = "Germany + France"; break;
        default:  regionStr = "UNK"; break;
    }

    this->gameInfo.append(regionStr);
    this->gameInfo.append(QString(regionFlag));
    this->gameInfo.append(""); //todo: display publisher
    this->gameInfo.append(gameType == 3 ? "" : makerCode);
    this->gameInfo.append(gameType == 3 ? "" : QString::number(discNumber));
    this->gameInfo.append(gameType == 3 ? "" : QString::number(discVersion));
    this->gameInfo.append(QString::number(streamingFlag));
    this->gameInfo.append(QString::number(streamingBufferSize));

    ReadProcessMemory(hook->getHandle(), reinterpret_cast<LPCVOID>(mramAddress+0xFC), &CPUSpeed, 4, nullptr);
    ReadProcessMemory(hook->getHandle(), reinterpret_cast<LPCVOID>(mramAddress+0xF8), &BUSSpeed, 4, nullptr);
    ReadProcessMemory(hook->getHandle(), reinterpret_cast<LPCVOID>(mramAddress+0x38), &FSTBegin, 4, nullptr);
    ReadProcessMemory(hook->getHandle(), reinterpret_cast<LPCVOID>(mramAddress+0x3c), &FSTSize, 4, nullptr);
    ReadProcessMemory(hook->getHandle(), reinterpret_cast<LPCVOID>(mramAddress+0x24), &version, 4, nullptr);

    CPUSpeed = DataConversion::swapBytes(CPUSpeed);
    BUSSpeed = DataConversion::swapBytes(BUSSpeed);
    FSTBegin = DataConversion::swapBytes(FSTBegin);
    FSTSize  = DataConversion::swapBytes(FSTSize);
    version  = DataConversion::swapBytes(version);

    this->gameInfo.append(QString::number(CPUSpeed/1000));
    this->gameInfo.append(QString::number(BUSSpeed/1000));
    this->gameInfo.append("0x" + QString::number(FSTBegin, 16));
    this->gameInfo.append("0x" + QString::number(FSTSize, 16));
    this->gameInfo.append(QString::number(version));

    if(gameType != 3)
    {
        switch(IDBuffer[0])
        {
            case 'G': case 'R': releaseType = "Official/Licensed Release"; break;
            case 'D': case 'P': releaseType = "Licensed Multi Game Disc/Demo/Preview/Kiosk"; break;
            case '3': case 'N': case 'E':releaseType = "Prototype/Dev Build"; break;
            default: releaseType = "Unknown/Hack"; break;
        }
    }
    this->gameInfo.append(releaseType);
}

void ProcessSelect::initializePJ64()
{
    BE = false;
    unsigned long long ramAddress = 0;

    memRegions.clear();
    for(unsigned long long i = 0; i < processMemRegions->size(); ++i)
    {
        unsigned long long msize = processMemRegions->at(i).get_size();
        if((msize == 0x400000) || (msize == 0x800000))
        {
            ramAddress = processMemRegions->at(i).get_address();
            unsigned int temp;
            ReadProcessMemory(hook->getHandle(), reinterpret_cast<LPCVOID>(ramAddress+0x8), &temp, 4, nullptr);
            if(temp == 0x03400008)
            {
                if(msize == 0x400000)
                { memRegions.push_back(MemRegion(ramAddress, 0x400000)); qDebug() << "size 4"; }
                else if(msize == 0x800000)
                { memRegions.push_back(MemRegion(ramAddress, 0x800000)); qDebug() << "size 8"; }
                //qDebug() << "N64 MEMORY FOUND";
                break;
            }
        }
    }
    rangeMap.clear();
    rangeMap.push_back(RangeLayout(ramAddress, 0x80000000, memRegions.at(0).get_size(), "RDRAM"));
}

void ProcessSelect::initializeCemu()
{
    BE = true;
    const unsigned int targetRangeSize[11] = { 0x000F0000, 0x00200000, 0x00400000, 0x4E000000, 0x02000000, 0x04000000,         0x02000000, 0x06000000,          0x00005000,  0x00005000,  0x00005000 };
    const unsigned int targetVAddr[11] =     { 0x00010000, 0x00E00000, 0x01800000, 0x02000000, 0x7C000000, 0xE0000000,         0xE8000000, 0xF4000000,          0xFFC00000,  0xFFC40000,  0xFFC80000 };
    const char* targetRangeLabels[11] =      { "Range 0",  "Range 1",  "JIT",      "App",      "Range 4",  "HW Communication", "Range 6",  "MEM1 -Kernel Heap", "Codegen 0", "Codegen 2", "Codegen 2" };

    unsigned long long ramAddress;
    unsigned int mRAMsize = 0x4E000000;
    unsigned long long msize;
    static unsigned long long mcount;
    memRegions.clear();
    rangeMap.clear();

    //searches for a specific range where all emulated ranges are expected to be found. This prevents targeting wrong memory regions
    for(unsigned long long i = 0; i < processMemRegions->size(); ++i)
    {
        msize = processMemRegions->at(i).get_size();
        if(msize == mRAMsize)
        {
            mcount = i - 16; //set counter 16 steps behind to ensure to catch all regions
            break;
        }
    }

    //browses all regions for each region information of targetRangeSize[]
    for(struct{ unsigned long long i = mcount; unsigned long long range = 0; }X; X.i < processMemRegions->size() && X.range < 11; ++X.i)
    {
        msize = processMemRegions->at(X.i).get_size();

        if(msize == targetRangeSize[X.range] && processMemRegions->at(X.i).getMapState() == MEM_COMMIT)
        {
            ramAddress = processMemRegions->at(X.i).get_address();

            if(msize == 0x4E000000)//the region of this size contains 2 memory regions. The app's executable files and the Game RAM
            {
                memRegions.push_back(MemRegion(ramAddress, targetVAddr[X.range]));
                rangeMap.push_back(RangeLayout(ramAddress, targetVAddr[X.range], 0x0E000000, "App"));
                memRegions.push_back(MemRegion(ramAddress+0x0E000000, 0x10000000));
                rangeMap.push_back(RangeLayout(ramAddress+0x0E000000, 0x10000000, 0x40000000, "Game RAM"));
            }
            else
            {
                memRegions.push_back(MemRegion(ramAddress, targetVAddr[X.range]));
                rangeMap.push_back(RangeLayout(ramAddress, targetVAddr[X.range], targetRangeSize[X.range], targetRangeLabels[X.range]));
            }
            X.range++;
        }
    }
}

void ProcessSelect::initializeGBA()
{
    BE = false;
    const unsigned long long targetRangeSize[8] = { 0x5000, 0x41000, 0x9000, 0x1000, 0x1000, 0x21000, 0x1000, 0x2001000 };

    for(int range = 0; range < 8; ++range) //for each range --- 8
    {
        for(unsigned long long i = 0; i < processMemRegions->size(); ++i)
        {
            unsigned long long msize = processMemRegions->at(i).get_size();
            unsigned long long ramAddress = processMemRegions->at(i).get_address();

            if(msize == targetRangeSize[range])
            {
                unsigned int test;
                ReadProcessMemory(hook->getHandle(), reinterpret_cast<LPCVOID>(ramAddress), &test, 4, nullptr);

                switch(range)
                {
                case 0:
                    if((test & 0xFF000000) == 0xEA000000)
                    {
                        qDebug() << "bios found";
                        memRegions.push_back(MemRegion(ramAddress, 0x00000000));
                        rangeMap.push_back(RangeLayout(ramAddress, 0x00000000, 0x4000, "BIOS"));
                        i = 0x7f000000;
                    }
                break;
                case 1:
                    qDebug() << "EWRAM found";
                    memRegions.push_back(MemRegion(ramAddress, 0x02000000));
                    rangeMap.push_back(RangeLayout(ramAddress, 0x02000000, 0x40000, "EWRAM"));
                    i = 0x7f000000;
                break;
                case 2:
                    if(test != 0 && processMemRegions->at(i).get_protectionVal() == PAGE_READWRITE && processMemRegions->at(i).get_address() > 0x00400000)
                    {
                        qDebug() << "IWRAM found";
                        memRegions.push_back(MemRegion(ramAddress, 0x03000000));
                        rangeMap.push_back(RangeLayout(ramAddress, 0x03000000, 0x8000, "IWRAM"));
                        i = 0x7f000000;
                    }
                break;
                case 3:
                    if(processMemRegions->at(i).get_protectionVal() == PAGE_READWRITE && processMemRegions->at(i).getMapState() == MEM_COMMIT && processMemRegions->at(i).getMemType() == MEM_MAPPED)//+type mapped
                    {
                        long long temp[2];
                        unsigned char temp2;
                        ReadProcessMemory(hook->getHandle(), reinterpret_cast<LPCVOID>(ramAddress+0x400), temp, 16, nullptr);
                        ReadProcessMemory(hook->getHandle(), reinterpret_cast<LPCVOID>(ramAddress), &temp2, 1, nullptr);
                        if(temp[0] == 0 && temp[1] == 0 && temp2 >= 0x40 && temp2 < 0x50)
                        {
                            qDebug() << "IORAM found";
                            memRegions.push_back(MemRegion(ramAddress, 0x04000000));
                            rangeMap.push_back(RangeLayout(ramAddress, 0x04000000, 0x400, "I/O Register"));
                            i = 0x7f000000;
                        }
                    }
                break;
                case 4:
                    if(processMemRegions->at(i).get_protectionVal() == PAGE_READWRITE && processMemRegions->at(i).getMapState() == MEM_COMMIT && processMemRegions->at(i).getMemType() == MEM_MAPPED)//+type mapped
                    {
                        long long temp;
                        ReadProcessMemory(hook->getHandle(), reinterpret_cast<LPCVOID>(ramAddress+0x8), &temp, 8, nullptr);
                        if(temp != 0)
                        {
                            qDebug() << "PALETTE found";
                            memRegions.push_back(MemRegion(ramAddress, 0x0500000));
                            rangeMap.push_back(RangeLayout(ramAddress, 0x05000000, 0x400, "PALLETTE RAM"));
                            i = 0x7f000000;
                        }
                    }
                break;
                case 5:
                    qDebug() << "VRAM found";
                    memRegions.push_back(MemRegion(ramAddress, 0x0600000));
                    rangeMap.push_back(RangeLayout(ramAddress, 0x06000000, 0x18000, "VRAM"));
                    i = 0x7f000000;
                break;
                case 6:
                    if(processMemRegions->at(i).get_address() > rangeMap.at(5).get_address())
                    {
                        qDebug() << "OAM found";
                        memRegions.push_back(MemRegion(ramAddress, 0x0700000));
                        rangeMap.push_back(RangeLayout(ramAddress, 0x07000000, 0x400, "OAM"));
                        i = 0x7f000000;
                    }
                break;
                case 7:
                    qDebug() << "ROM found";
                    memRegions.push_back(MemRegion(ramAddress, 0x0800000));
                    rangeMap.push_back(RangeLayout(ramAddress, 0x08000000, 0x2000000, "ROM"));
                    i = 0x7f000000;
                break;
                }

                //gameTitle = fetchGameTitle(hook);
            }
        }
    }
}

void ProcessSelect::initializeGB() //TODO: find a way to find the emulated mem maps without pointers (it's not forward compatible this way)
{
    BE = false;
    for(int range = 0; range < 5; ++range)
    {
        for(unsigned long long i = 0; i < processMemRegions->size(); ++i)
        {
            unsigned long long msize = processMemRegions->at(i).get_size();
            unsigned long long ramAddress = processMemRegions->at(i).get_address();

                switch(range)// TODO: find wram0, wram1
                {
                case 0:
                    if(msize >= 0xB0000 && msize <= 0xF0000 && processMemRegions->at(i).get_protectionVal() == PAGE_READWRITE)
                    {
                        unsigned int* mapCopy = reinterpret_cast<unsigned int*>(malloc(msize));
                        ReadProcessMemory(hook->getHandle(), reinterpret_cast<LPCVOID>(ramAddress), mapCopy, msize, nullptr);

                        for(unsigned long long x = 0; x < msize/4; ++x)
                        {
                            if(*(mapCopy+x) == 0x6666EDCE)
                            {
                                qDebug() << "ROM bank 1 found";
                                memRegions.push_back(MemRegion(ramAddress+x*4-0x104, 0x00000000));
                                rangeMap.push_back(RangeLayout(ramAddress+x*4-0x104, 0x00000000, 0x4000, "ROM Bank 0"));
                                memRegions.push_back(MemRegion(ramAddress+x*4+0x3EFC, 0x00004000));
                                rangeMap.push_back(RangeLayout(ramAddress+x*4+0x3EFC, 0x00004000, 0x4000, "ROM Bank 1"));
                                memRegions.push_back(MemRegion(ramAddress+x*4-0x104, 0x00000000));
                                rangeMap.push_back(RangeLayout(ramAddress+x*4-0x104, 0x00000000, 0x8000, "ROM Bank 0&1"));
                                i = 0x7f000000;
                                goto done;
                            }
                        }
                        done:
                        delete mapCopy;
                    }
                break;
                case 1:
                {
                    LPCVOID ptr = reinterpret_cast<LPCVOID>(0x00755e68); //VBA.exe (0x00400000) + 0x00355e68. pointer here points to VRAM
                    unsigned long long temp = 0;
                    ReadProcessMemory(hook->getHandle(), ptr, &temp, 4, nullptr);
                    qDebug() << "vram found";
                    memRegions.push_back(MemRegion(temp, 0x8000));
                    rangeMap.push_back(RangeLayout(temp, 0x8000, 0x2000, "VRAM"));
                    i = 0x7f000000;
                }
                break;
                case 2:
                {
                    LPCVOID ptr = reinterpret_cast<LPCVOID>(0x00755e70); //VBA.exe (0x00400000) + 0x00355e70. pointer here points to SRAM
                    unsigned long long temp = 0;
                    ReadProcessMemory(hook->getHandle(), ptr, &temp, 4, nullptr);
                    qDebug() << "s/e ram found";
                    memRegions.push_back(MemRegion(temp, 0xA000));
                    rangeMap.push_back(RangeLayout(temp, 0xA000, 0x2000, "ERAM"));
                    i = 0x7f000000;
                }
                break;
                case 3:
                {
                    LPCVOID ptr = reinterpret_cast<LPCVOID>(0x00755e78); //VBA.exe (0x00400000) + 0x00355e78. pointer here points to WRAM
                    unsigned long long temp = 0;
                    ReadProcessMemory(hook->getHandle(), ptr, &temp, 4, nullptr);
                    qDebug() << "w ram found";
                    memRegions.push_back(MemRegion(temp, 0xC000));
                    rangeMap.push_back(RangeLayout(temp, 0xC000, 0x1000, "WRAM Bank 0"));
                    memRegions.push_back(MemRegion(temp+ 0x1000, 0xD000));
                    rangeMap.push_back(RangeLayout(temp+ 0x1000, 0xD000, 0x1000, "WRAM Bank 1"));
                    memRegions.push_back(MemRegion(temp, 0xC000));
                    rangeMap.push_back(RangeLayout(temp, 0xC000, 0x2000, "WRAM Bank 0&1"));
                    i = 0x7f000000;
                }
                break;
                case 4:
                {
                    LPCVOID ptr = reinterpret_cast<LPCVOID>(0x00755e84); //VBA.exe (0x00400000) + 0x00355e84. pointer here points to IO registers
                    unsigned long long temp = 0;
                    ReadProcessMemory(hook->getHandle(), ptr, &temp, 4, nullptr);
                    qDebug() << "io ram found";
                    memRegions.push_back(MemRegion(temp+0xf00, 0x0000FF00));
                    rangeMap.push_back(RangeLayout(temp+0xf00, 0x0000FF00, 0x80, "I/O Registers"));
                    memRegions.push_back(MemRegion(temp+0xf80, 0x0000FF80));
                    rangeMap.push_back(RangeLayout(temp+0xf80, 0x0000FF80, 0x80, "HRAM + IE"));
                    i = 0x7f000000;
                }
                break;
                }
        }
    }
}

void ProcessSelect::initializeGBC()
{
    BE = false;
    for(int range = 0; range < 7; ++range) //for each range --- 8
    {
        for(unsigned long long i = 0; i < processMemRegions->size(); ++i)
        {
            unsigned long long msize = processMemRegions->at(i).get_size();
            unsigned long long ramAddress = processMemRegions->at(i).get_address();

                switch(range)// TODO: find wram0, wram1
                {
                case 0:
                    if(msize >= 0x1a0000 && msize <= 0x300000 && processMemRegions->at(i).get_protectionVal() == PAGE_READWRITE)
                    {
                        unsigned int* mapCopy = reinterpret_cast<unsigned int*>(malloc(msize));
                        ReadProcessMemory(hook->getHandle(), reinterpret_cast<LPCVOID>(ramAddress), mapCopy, msize, nullptr);

                        for(unsigned long long x = 0; x < msize/4; ++x)
                        {
                            if(*(mapCopy+x) == 0x6666EDCE)
                            {
                                qDebug() << "ROM bank 1 found";
                                memRegions.push_back(MemRegion(ramAddress+x*4-0x104, 0x00000000));
                                rangeMap.push_back(RangeLayout(ramAddress+x*4-0x104, 0x00000000, 0x4000, "ROM Bank 0"));
                                i = 0x7f000000;
                                goto done;
                            }
                        }
                        done:
                        delete mapCopy;

                    }
                break;
                case 1:
                {
                    LPCVOID ptr = reinterpret_cast<LPCVOID>(0x00755e58); //VBA.exe (0x00400000) + 0x00355e58. pointer here points to ROM bank 1
                    unsigned long long temp = 0;
                    ReadProcessMemory(hook->getHandle(), ptr, &temp, 4, nullptr);
                    qDebug() << "vram found";
                    memRegions.push_back(MemRegion(temp, 0x4000));
                    rangeMap.push_back(RangeLayout(temp, 0x4000, 0x4000, "ROM Bank 1"));
                    i = 0x7f000000;
                }
                break;
                case 2:
                {
                    LPCVOID ptr = reinterpret_cast<LPCVOID>(0x00755e68); //VBA.exe (0x00400000) + 0x00355e68. pointer here points to VRAM
                    unsigned long long temp = 0;
                    ReadProcessMemory(hook->getHandle(), ptr, &temp, 4, nullptr);
                    qDebug() << "vram found";
                    memRegions.push_back(MemRegion(temp, 0x8000));
                    rangeMap.push_back(RangeLayout(temp, 0x8000, 0x2000, "VRAM"));
                    i = 0x7f000000;
                }
                break;
                case 3:
                {
                    LPCVOID ptr = reinterpret_cast<LPCVOID>(0x00755e70); //VBA.exe (0x00400000) + 0x00355e70. pointer here points to SRAM
                    unsigned long long temp = 0;
                    ReadProcessMemory(hook->getHandle(), ptr, &temp, 4, nullptr);
                    qDebug() << "s/e ram found";
                    memRegions.push_back(MemRegion(temp, 0xA000));
                    rangeMap.push_back(RangeLayout(temp, 0xA000, 0x2000, "ERAM"));
                    i = 0x7f000000;
                }
                break;
                case 4:
                {
                    LPCVOID ptr = reinterpret_cast<LPCVOID>(0x00755e78); //VBA.exe (0x00400000) + 0x00355e78. pointer here points to WRAM
                    unsigned long long temp = 0;
                    ReadProcessMemory(hook->getHandle(), ptr, &temp, 4, nullptr);
                    qDebug() << "w ram found";
                    memRegions.push_back(MemRegion(temp, 0xC000));
                    rangeMap.push_back(RangeLayout(temp, 0xC000, 0x1000, "WRAM Bank 0"));
                    i = 0x7f000000;
                }
                break;
                case 5:
                {
                    LPCVOID ptr = reinterpret_cast<LPCVOID>(0x00755e7C); //VBA.exe (0x00400000) + 0x00355e78. pointer here points to WRAM
                    unsigned long long temp = 0;
                    ReadProcessMemory(hook->getHandle(), ptr, &temp, 4, nullptr);
                    qDebug() << "w ram found";
                    memRegions.push_back(MemRegion(temp, 0xD000));
                    rangeMap.push_back(RangeLayout(temp, 0xD000, 0x1000, "WRAM Bank 1"));
                    i = 0x7f000000;
                }
                break;
                case 6:
                {
                    LPCVOID ptr = reinterpret_cast<LPCVOID>(0x00755e84); //VBA.exe (0x00400000) + 0x00355e84. pointer here points to IO registers
                    unsigned long long temp = 0;
                    ReadProcessMemory(hook->getHandle(), ptr, &temp, 4, nullptr);
                    qDebug() << "io ram found";
                    memRegions.push_back(MemRegion(temp+0xf00, 0x0000FF00));
                    rangeMap.push_back(RangeLayout(temp+0xf00, 0x0000FF00, 0x80, "I/O Registers"));
                    memRegions.push_back(MemRegion(temp+0xf80, 0x0000FF80));
                    rangeMap.push_back(RangeLayout(temp+0xf80, 0x0000FF80, 0x80, "HRAM + IE"));
                    i = 0x7f000000;
                }
                break;
                }
        }
    }
}

void ProcessSelect::initializeDesmume()
{
    BE = false;
    memRegions.clear();
    for(unsigned long long i = 0; i < processMemRegions->size(); ++i)
    {
        unsigned long long msize = processMemRegions->at(i).get_size();
        unsigned long long ramAddress = processMemRegions->at(i).get_address();

        if(ramAddress > 0x141000000 && ramAddress < 0x200000000 && msize > 0x2000000 && msize < 0x10000000 && (processMemRegions->at(i).get_protectionVal() == PAGE_READWRITE || processMemRegions->at(i).get_protectionVal() == PAGE_EXECUTE_READWRITE || processMemRegions->at(i).get_protectionVal() == PAGE_EXECUTE_WRITECOPY))
        {
            unsigned int* mapCopy = reinterpret_cast<unsigned int*>(malloc(msize));
            ReadProcessMemory(hook->getHandle(), reinterpret_cast<LPCVOID>(ramAddress), mapCopy, msize, nullptr);

            for(unsigned long long j = 0; j < (msize/4); ++j)
            {
                if(*(mapCopy+j) == 0xE7FFDEFF)
                {
                    //qDebug() << "DS Memory found! ";
                    memRegions.push_back(MemRegion(ramAddress+ j*4, 0x1000000));
                    rangeMap.push_back(RangeLayout(ramAddress+ j*4, 0x02000000, 0x1000000, "MRAM"));
                    ++i;
                    goto done;
                }
            }
            done:
            delete mapCopy;
        }

        if(ramAddress > 0x141000000 && ramAddress < 0x200000000 && msize > 0x2000000 && msize < 0x10000000)
        {
            unsigned int* mapCopy = reinterpret_cast<unsigned int*>(malloc(msize));

            ReadProcessMemory(hook->getHandle(), reinterpret_cast<LPCVOID>(ramAddress), mapCopy, msize, nullptr);

            for(unsigned long long j = 0; j < (msize/4); ++j)
            {
                if(*(mapCopy+j) == 0x5043414D)
                {
                    //qDebug() << "Firmware found! ";
                    memRegions.push_back(MemRegion((ramAddress+ j*4) - 8, 0x00040000));
                    rangeMap.push_back(RangeLayout((ramAddress+ j*4) - 8, 0x00000000, 0x00040000, "BIOS"));
                    ++i;
                    goto done2;
                }
            }
            done2:
            delete mapCopy;
        }
    }
}

void ProcessSelect::initializeCitra()
{
    BE = false;
    unsigned long long ramAddress = 0;

    memRegions.clear();
    for(unsigned long long i = 0; i < processMemRegions->size(); ++i)
    {
        unsigned long long msize = processMemRegions->at(i).get_size();
        if(msize == 0x10001000)
        {
            ramAddress = processMemRegions->at(i).get_address();
            memRegions.push_back(MemRegion(ramAddress+0x6D96040, 0x10000000));
            rangeMap.push_back(RangeLayout(ramAddress+0x6D96040, 0x0, 0x10000000, "Main Memory"));
        }
    }
}

void ProcessSelect::initializeMM()
{
    BE = true;
    LPCVOID ptr = reinterpret_cast<LPCVOID>(0x006A52D8); //Fusion.exe (0x00400000) + 0x002A52D8. pointer here points to pointer which points to emulated RAM
    unsigned int buffer = 0;
    ReadProcessMemory(hook->getHandle(), ptr, &buffer, 4, nullptr); //reads pointer that redirects to the emulated RAM
    qDebug() << QString::number(buffer, 16);

    memRegions.push_back(MemRegion(buffer, 0x0400));
    rangeMap.push_back(RangeLayout(buffer, 0x0000, 0x0400, "ROM unpaged"));
    memRegions.push_back(MemRegion(buffer+0x0400, 0x3C00));
    rangeMap.push_back(RangeLayout(buffer+0x0400, 0x0400, 0x3C00, "ROM mapper slot 0"));
    memRegions.push_back(MemRegion(buffer+0x4000, 0x4000));
    rangeMap.push_back(RangeLayout(buffer+0x4000, 0x4000, 0x4000, "ROM mapper slot 1"));
    memRegions.push_back(MemRegion(buffer+0x8000, 0x4000));
    rangeMap.push_back(RangeLayout(buffer+0x8000, 0x8000, 0x4000, "ROM/RAM mapper slot 2"));
    memRegions.push_back(MemRegion(buffer+0xC000, 0x2000));
    rangeMap.push_back(RangeLayout(buffer+0xC000, 0xC000, 0x2000, "System RAM"));
    memRegions.push_back(MemRegion(buffer+0xE000, 0x2000));
    rangeMap.push_back(RangeLayout(buffer+0xE000, 0xE000, 0x2000, "System RAM Mirror"));
}

void ProcessSelect::initializeGG()
{
    BE = true;
    LPCVOID ptr = reinterpret_cast<LPCVOID>(0x006A52D8); //Fusion.exe (0x00400000) + 0x002A52D8. pointer here points to pointer which points to emulated RAM
    unsigned int buffer = 0;
    ReadProcessMemory(hook->getHandle(), ptr, &buffer, 4, nullptr); //reads pointer that redirects to the emulated RAM
    qDebug() << QString::number(buffer, 16);

    memRegions.push_back(MemRegion(buffer, 0xC000));
    rangeMap.push_back(RangeLayout(buffer, 0x0000, 0xC000, "ROM, misc."));
    memRegions.push_back(MemRegion(buffer+0xC000, 0x2000));
    rangeMap.push_back(RangeLayout(buffer+0xC000, 0xC000, 0x2000, "System RAM"));
    memRegions.push_back(MemRegion(buffer+0xE000, 0x2000));
    rangeMap.push_back(RangeLayout(buffer+0xE000, 0xE000, 0x2000, "System RAM Mirror"));
}

void ProcessSelect::initializeMD()
{
    BE = true;
    LPCVOID ptr = reinterpret_cast<LPCVOID>(0x00759F14); //Fusion.exe (0x00400000) + 0x00359F14. pointer here points to pointer which points to emulated RAM
    unsigned int buffer = 0;
    ReadProcessMemory(hook->getHandle(), ptr, &buffer, 4, nullptr); //reads pointer that redirects to the emulated RAM
    qDebug() << QString::number(buffer, 16);

    memRegions.push_back(MemRegion(buffer, 0x10000));
    rangeMap.push_back(RangeLayout(buffer, 0xFF0000, 0x10000, "System RAM"));
}

void ProcessSelect::initializeCD()
{
    BE = true;
    LPCVOID ptr = reinterpret_cast<LPCVOID>(0x006A52D4); //Fusion.exe (0x00400000) + 0x002A52D4. pointer here points to pointer which points to emulated RAM
    unsigned int buffer = 0;
    ReadProcessMemory(hook->getHandle(), ptr, &buffer, 4, nullptr); //reads pointer that redirects to the emulated RAM
    qDebug() << QString::number(buffer, 16);

    memRegions.push_back(MemRegion(buffer, 0x10000));
    rangeMap.push_back(RangeLayout(buffer, 0xFF0000, 0x10000, "Main CPU RAM"));
}

void ProcessSelect::initializeYabause()
{
    BE = false;
    unsigned long long ramAddress = 0;
    int count = 0;
    memRegions.clear();
    for(unsigned long long i = 0; i < processMemRegions->size(); ++i)
    {
        unsigned long long msize = processMemRegions->at(i).get_size();
        if(msize == 0x101000)
        {
            ++count;
            ramAddress = processMemRegions->at(i).get_address();
            memRegions.push_back(MemRegion(ramAddress+0x40, 0x100000));
            (count == 1) ? rangeMap.push_back(RangeLayout(ramAddress+0x40, 0x06000000, 0x100000, "Work RAM High"))
                         : rangeMap.push_back(RangeLayout(ramAddress+0x40, 0x00200000, 0x100000, "Work RAM Low"));
        }

        if(count == 2){ return; }
    }
}

void ProcessSelect::initializeNullDC()
{
    BE = false;
    unsigned long long ramAddress = 0;
    memRegions.clear();
    for(unsigned long long i = 0; i < processMemRegions->size(); ++i)
    {
        unsigned long long msize = processMemRegions->at(i).get_size();
        if(msize == 0x1000000)
        {
            ramAddress = processMemRegions->at(i).get_address();
            memRegions.push_back(MemRegion(ramAddress, 0x1000000));
            rangeMap.push_back(RangeLayout(ramAddress, 0x0C000000, 0x1000000, "Main Memory"));
            return;
        }
    }
}

void ProcessSelect::initializeEPSXE()
{
    BE = false;
    memRegions.clear();
    for(unsigned long long i = 0; i < processMemRegions->size(); ++i)
    {
        unsigned long long msize = processMemRegions->at(i).get_size();
        unsigned long long ramAddress = processMemRegions->at(i).get_address();

        if(msize > 0x1000000)
        {
            unsigned int* mapCopy = reinterpret_cast<unsigned int*>(malloc(msize));
            ReadProcessMemory(hook->getHandle(), reinterpret_cast<LPCVOID>(ramAddress), mapCopy, msize, nullptr);

            for(unsigned long long j = 0; j < (msize/4); ++j)
            {
                if(*(reinterpret_cast<unsigned long long*>(mapCopy+j)) == 0x03400008275a0c80)
                {
                    qDebug() << "ps1 Memory found!---";
                    memRegions.push_back(MemRegion(ramAddress+ j*4 -4, 0x200000));
                    rangeMap.push_back(RangeLayout(ramAddress+ j*4 -4, 0, 0x200000, "Main RAM"));
                    ++i;
                    qDebug() << "ffadfgsdf " << rangeMap[0].get_virtualAddress();
                    goto done;
                }
            }
            done:
            delete mapCopy;
            return;
        }
    }
}

void ProcessSelect::initializePCSX2()
{
    BE = false;
    memRegions.clear();
    rangeMap.clear();
    memRegions.push_back(MemRegion(0x20000000, 0x2000000));
    rangeMap.push_back(RangeLayout(0x20000000, 0x00000000, 0x2000000, "Main Memory"));
}

void ProcessSelect::initializeRPCS3()
{
    BE = true;
    unsigned long long ramAddress = 0;
    int count = 1;

    memRegions.clear();

    for(unsigned long long i = 0; i < processMemRegions->size() && processMemRegions->at(i).get_address() < 0x700000000000; ++i)
    {
        if(processMemRegions->at(i).get_address() >= 0x300000000 && (processMemRegions->at(i).getMemType() == MEM_MAPPED || processMemRegions->at(i).getMemType() == MEM_IMAGE))
        {

            //qDebug() << processMemRegions->at(i).get_protectionVal() << " - " << processMemRegions->at(i).get_protectionStr();
            ramAddress = processMemRegions->at(i).get_address();
            unsigned long long msize = 0;

            while(processMemRegions->at(i).getMemType() == MEM_MAPPED || processMemRegions->at(i).getMemType() == MEM_IMAGE)
            {
                msize += processMemRegions->at(i).get_size();
                ++i;
                qDebug() << QString::number(processMemRegions->at(i).getMemType(), 16);
            }

            rangeMap.push_back(RangeLayout(ramAddress, ramAddress, msize, "Game OS " + QString::number(count)));
            memRegions.push_back(MemRegion(ramAddress, msize));
            ++count;
        }
    }
}

void ProcessSelect::initializePPSSPP()
{
    BE = false;
    unsigned long long ramAddress = 0;
    memRegions.clear();
    for(unsigned long long i = 0; i < processMemRegions->size(); ++i)
    {
        unsigned long long msize = processMemRegions->at(i).get_size();
        if(msize == 0x1f00000)
        {
            ramAddress = processMemRegions->at(i).get_address();
            memRegions.push_back(MemRegion(ramAddress+0x800000, 0x1800000));
            rangeMap.push_back(RangeLayout(ramAddress+0x800000, 0x00000000, 0x1800000, "Main Memory"));
            return;
        }
    }
}

void ProcessSelect::initializePC()
{
    BE = false;
    unsigned long long ramAddress = 0;
    memRegions.clear();
    for(unsigned long long i = 0; i < processMemRegions->size(); ++i)
    {
        if(processMemRegions->at(i).getMemType() == MEM_MAPPED || processMemRegions->at(i).getMemType() == MEM_IMAGE)
        {
            ramAddress = processMemRegions->at(i).get_address();
            memRegions.push_back(MemRegion(ramAddress, ramAddress));
            rangeMap.push_back(RangeLayout(ramAddress, ramAddress, processMemRegions->at(i).get_size(), QString::number(ramAddress, 16)));
        }
    }
}
