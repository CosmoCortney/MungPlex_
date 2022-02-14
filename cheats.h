#ifndef CHEATS_H
#define CHEATS_H
#include<string>
#include<lua.hpp>
#include<QDebug>
#include<windows.h>
#include<QString>
#include"rangelayout.h"
#include"dataconversion.h"
#include"hook.h"
#include<QTimer>
#include<QDir>
#include<QFile>
#include<QMessageBox>
#include<QMetaObject>
#include<QObject>

class Cheats : public QObject
{
    Q_OBJECT

private:
    /*explicit*/ Cheats(/*QObject* parent*/);
    //static Cheats sInstance;
    std::string script;
    std::string result;
    Hook hook;
    std::vector<RangeLayout> rangeLayouts;
    inline static QStringList cheatList;
    inline static QStringList cheatScripts;
    inline static QDir cheatsPath;

    inline static std::string sscript;
    inline static std::string sresult;
    inline static std::string logStr;
    inline static Hook shook;
    inline static HANDLE snap;
    inline static std::vector<RangeLayout> srangeLayouts;
    inline static lua_State* L;

public:
    virtual ~Cheats(){ }
    Cheats(const Cheats&) = delete;
    static Cheats& getInstance()
    {
        static Cheats sInstance;
        return sInstance;
    }


    static bool checkLua(lua_State* L, int val);
    QString logLuaError(std::string& err);
    void sendLuaCheat();
    QString logLua(std::string& result);
    void setCheat(std::string& val){ script = val; }
    static std::string* getErrorStr();

    static void routine(lua_State* L);
    static int writeToRAM(lua_State* L);
    static int readFromRAM(lua_State* L);
    static int logToGUI(lua_State* L);
    static int clearLog(lua_State* L);
    void setupCheatList(QStringList& cList, QDir& cPath) { cheatList = cList; cheatsPath = cPath; }
    static void loadCheatsFromDisk();
    void setHook(Hook& h){ hook = h; }
    void set_rangeLayouts(std::vector<RangeLayout>& val){ rangeLayouts = val; }
    static std::string* getLogStr();
    void processSnapshot();

public slots:
    //slots to be called from the MainWindow class
    static void executeCheat();
    static void executeCheatList();

private slots:
    //static inline void test1(){ emit error(); }



signals:
    void error();
    void log();
    void clear();
};

//inline Cheats Cheats::sInstance;

#endif // CHEATS_H
