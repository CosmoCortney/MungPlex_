#include "cheats.h"
#include "mainwindow.h"

/*This class provides functions to execute cheat codes as Lua scripts!
 * Some special functions and parameters are defined:
 *
 * ReadFromRAM(TYPE, ADDRESS)
 * This function reads a value of TYPE from ADDRESS and returns the found value.
 * If the value is nil the address was out of bounds or no game is currently running
 *
 * WriteToRAM(TYPE, ADDRESS, VALUE)
 * This functions write VALUE of TYPE to ADDRESS.
 *
 * LogToGUI(val)
 * Logs data to GUI
 *
 * ClearLog()
 * Clears log text field on GUI
 *
 * Parameters for TYPE
 * BOOL: VALUE is boolean
 * INT8: VALUE is 8-bit integer (value is never negative because it only occupies 8 bits of the Lua integer (64 bits))
 * INT16: VALUE is 16-bit integer (value is never negative because it only occupies 16 bits of the Lua integer (64 bits))
 * INT24: VALUE is 24-bit integer (experimental, can never be interpreted as negative Lua integer)
 * INT32: VALUE is 32-bit integer (value is never negative because it only occupies 8 bits of the Lua integer (64 bits))
 * INT64: VALUE is 64-bit integer
 * INT96: VALUE is 96-bit integer (not yet implemented)
 * INT128: VALUE is 128-bit integer (not yet implemented)
 * FLOAT: VALUE is float (the value is read as float and then converted to Lua number to be usable)
 * DOUBLE: VALUE is double
 *
 *
 * Parameters for ADDRESS
 * The logical address the emulated system operates with.
 *
 *
 * Parameters for VALUE
 * Must be compatible to TYPE. Otherwise unexpected behavior may occurre
 *
 *
 *
 * Example cheat
 *
 * coins = ReadFromRAM(INT16, 0x810EC462)
 * coins = coins + 8
 * WriteToRAM(FLOAT, 0x810EC474, coins)
 *
 * This example cheat Read a 16-bit integer from address 0x810EC462 and stores it inside the variable coins
 * then coins becomes incremented by 8
 * ten coins is written to 0x810EC474 as float
 *
*/

/*cheatTimer = new QTimer(this);
cheatTimerList = new QTimer(this);
connect(cheatTimer, &QTimer::timeout, &Cheats::executeCheat);*/
//#include"main.cpp"
Cheats::Cheats(/*QObject* parent*/)
{
    //this->parent = parent;
    //connect(this, SIGNAL(error()), this->parent, SLOT(cancel()));
}



//void Cheats::emitError()
//{
//    emit Cheats::getInstance().error();
//}

enum typeValues {BOOL_ = 0, INT8_ = 1,  INT16_ = 2,  INT24_ = 3,  INT32_ = 4,  INT64_ = 5,  INT96_ = 6,  INT128_ = 7,  FLOAT_ = 8,  DOUBLE_ = 9 };

bool Cheats::checkLua(lua_State* L, int val)
{
    if(val != LUA_OK)
    {
        sresult.append(lua_tostring(L, -1));
        return false;
    }
    return true;
}

QString Cheats::logLuaError(std::string& err)
{
    return QString::fromStdString(err);
}


void Cheats::sendLuaCheat()
{
    L = luaL_newstate();
    luaL_openlibs(L);
    sscript = script;
    shook = hook;
    srangeLayouts = rangeLayouts;

    lua_register(L, "WriteToRAM", writeToRAM);
    lua_register(L, "ReadFromRAM", readFromRAM);
    lua_register(L, "LogToGUI", logToGUI);
    lua_register(L, "ClearLog", clearLog);
    lua_pushinteger(L, BOOL_);
    lua_setglobal(L, "BOOL");
    lua_pushinteger(L, INT8_);
    lua_setglobal(L, "INT8");
    lua_pushinteger(L, INT16_);
    lua_setglobal(L, "INT16");
    lua_pushinteger(L, INT24_);
    lua_setglobal(L, "INT24");
    lua_pushinteger(L, INT32_);
    lua_setglobal(L, "INT32");
    lua_pushinteger(L, INT64_);
    lua_setglobal(L, "INT64");
    lua_pushinteger(L, INT96_);
    lua_setglobal(L, "INT96");
    lua_pushinteger(L, INT128_);
    lua_setglobal(L, "INT128");
    lua_pushinteger(L, FLOAT_);
    lua_setglobal(L, "FLOAT");
    lua_pushinteger(L, DOUBLE_);
    lua_setglobal(L, "DOUBLE");

    for(int i = 0; i < 32; ++i)
    {
        std::string nr = ((i<10) ? std::to_string(0).append(std::to_string(i)) : std::to_string(i));
        std::string name = std::string("INTREG").append(nr);
        lua_pushinteger(L, 0);
        lua_setglobal(L, name.c_str());

        name = std::string("NUMREG").append(nr);
        lua_pushnumber(L, 0);
        lua_setglobal(L, name.c_str());

        name = std::string("BOOLREG").append(nr);
        lua_pushboolean(L, false);
        lua_setglobal(L, name.c_str());
    }


}

void Cheats::routine(lua_State* L)
{
    sresult.clear();
    logStr.clear();
    lua_register(L, "WriteToRAM", writeToRAM);
    lua_register(L, "ReadFromRAM", readFromRAM);
    lua_register(L, "LogToGUI", logToGUI);
}

void Cheats::executeCheat()
{
    if(checkLua(L, luaL_dostring(L, sscript.c_str())))
    {

    }
    else
    {
        sresult = "Error Text-Cheat: ";
        sresult.append(lua_tostring(L, -1));
        emit Cheats::getInstance().error();
    }
}

void Cheats::executeCheatList()
{
    for(int cheatNr = 0; cheatNr < cheatScripts.count(); ++cheatNr)
    {
        sscript = cheatScripts.at(cheatNr).toStdString();

        if(checkLua(L, luaL_dostring(L, sscript.c_str())))
        {

        }
        else
        {
            sresult = "Error Cheat List at cheat #: ";
            sresult.append(std::to_string(cheatNr+1) + ": " + lua_tostring(L, -1));
            emit Cheats::getInstance().error();
        }
    }
}

void Cheats::loadCheatsFromDisk()
{
    cheatScripts.clear();

    for(int cheatNr = 0; cheatNr < cheatList.count(); ++cheatNr)
    {
        QFile cheatFile(cheatsPath.path() + "/" + cheatList.at(cheatNr));
        if(!cheatFile.open(QIODevice::ReadOnly)) { QMessageBox::warning(0, "error", cheatFile.errorString()); return; }
        QTextStream in(&cheatFile);
        cheatScripts.append(in.readAll());
        cheatFile.close();
    }

    for(int i = 0; i < cheatScripts.count(); ++i)
    {
        qDebug() << cheatScripts.at(i);
    }
}

std::string* Cheats::getErrorStr()
{ return &sresult; }

std::string* Cheats::getLogStr()
{ return &logStr; }


QString Cheats::logLua(std::string& result)
{
    return QString::fromStdString(result);
}

int Cheats::readFromRAM(lua_State* L) //readFromRAM(TYPE, addr)
{
    long long type = lua_tointeger(L, 1);
    unsigned long long addr = static_cast<unsigned long long>(lua_tointeger(L, 2));
    unsigned long long val;
    unsigned long long size;

    for(unsigned long long i = 0; i < srangeLayouts.size(); ++i)
    {
        if(   ((addr < (srangeLayouts[i].get_virtualAddress() + srangeLayouts[i].get_size()))     &&     (addr >= srangeLayouts[i].get_virtualAddress())) || (srangeLayouts[i].get_virtualAddress() == srangeLayouts[i].get_address()))
        {
            switch (type)
            {
                case BOOL_: case INT8_: size = 1; break;
                case INT16_: size = 2; break;
                case INT24_: size = 3; break;
                case FLOAT_: case INT32_: size = 4; break;
                default: size = 4; break;
            }

            ReadProcessMemory(shook.getHandle(), reinterpret_cast<LPCVOID>((addr-srangeLayouts[i].get_virtualAddress())+srangeLayouts[i].get_address()), &val, size, nullptr);
            unsigned long long cleaner = 0xFFFFFFFFFFFFFFFF;

            switch(type)
            {
            case BOOL_: case INT8_: cleaner &= 0x00000000000000FF; size = 1; break;
            case INT16_: cleaner &= 0x000000000000FFFF; size = 2; break;
            case INT24_: cleaner &= 0x0000000000FFFFFF; size = 3; break;
            case FLOAT_: case INT32_: cleaner &= 0x00000000FFFFFFFF; size = 4; break;
            default: size = 8;
            }

            if(ProcessSelect::getInstance().isBE())
            {
                val = DataConversion::swapBytes(val);

                switch(type)
                {
                case BOOL_: case INT8_: val >>= 56; break;
                case INT16_: val >>= 48; break;
                case INT24_: val >>= 40; break;
                case FLOAT_: case INT32_: val >>= 32; break;
                }
            }

            val &= cleaner;

            switch(type)
            {
                case BOOL_: lua_pushboolean(L, static_cast<bool>(val)); break;
                case FLOAT_:
                {
                    double temp = (double)*(float*)&val;
                    lua_pushnumber(L, temp);
                }
                break;
                case DOUBLE_:
                {
                    double temp = *(double*)&val;
                    lua_pushnumber(L, temp);
                }
                break;
                default: lua_pushinteger(L, val); break;
            }
            return 1;
        }
    }
    return 0;
}

int Cheats::writeToRAM(lua_State* L)
{
    long long type = lua_tointeger(L, 1);
    unsigned long long addr = static_cast<unsigned long long>(lua_tointeger(L, 2));
    unsigned long long val;
    unsigned long long size;

    switch (type)
    {
        case BOOL_: val = lua_toboolean(L, 3); break;
        case FLOAT_:
        {
            float temp = (float)lua_tonumber(L, 3);
           // qDebug() << temp;
            val = *(int*)&temp;
    }
        break;
        case DOUBLE_:
        {
            double temp = lua_tonumber(L, 3);
            val = *(long long*)&temp;
    }
        break;
        default: val = lua_tointeger(L, 3); break;
    }

    for(int i = 0; i < srangeLayouts.size(); ++i)
    {
        if(   ((addr < (srangeLayouts[i].get_virtualAddress() + srangeLayouts[i].get_size()))     &&     (addr >= srangeLayouts[i].get_virtualAddress())) || (srangeLayouts[i].get_virtualAddress() == srangeLayouts[i].get_address()))
        {
            unsigned long long cleaner = 0xFFFFFFFFFFFFFFFF;

            switch(type)
            {
            case BOOL_: case INT8_: cleaner &= 0x00000000000000FF; size = 1; break;
            case INT16_: cleaner &= 0x000000000000FFFF; size = 2; break;
            case INT24_: cleaner &= 0x0000000000FFFFFF; size = 3; break;
            case FLOAT_: case INT32_: cleaner &= 0x00000000FFFFFFFF; size = 4; break;
            default: size = 8;
            }

            if(ProcessSelect::getInstance().isBE())
            {
                val = DataConversion::swapBytes(val);

                switch(type)
                {
                case BOOL_: case INT8_: val >>= 56; break;
                case INT16_: val >>= 48; break;
                case INT24_: val >>= 40; break;
                case FLOAT_: case INT32_: val >>= 32; break;
                }
            }

            val &= cleaner;
            WriteProcessMemory(shook.getHandle(), reinterpret_cast<LPVOID>((addr-srangeLayouts[i].get_virtualAddress())+srangeLayouts[i].get_address()), &val, size, nullptr);
            return 1;
        }
    }
    return 0;
}

int Cheats::logToGUI(lua_State* L)
{
    logStr = lua_tostring(L, 1);
    emit Cheats::getInstance().log();
    return 1;
}

int Cheats::clearLog(lua_State* L)
{
    emit Cheats::getInstance().clear();
    return 1;
}
