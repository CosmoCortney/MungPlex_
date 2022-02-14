#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#define VER 3

#include <QMainWindow>
#include<windows.h>
#include<TlHelp32.h>
#include<vector>
#include<QDebug>
#include<QString>
#include"hook.h"
#include"memregion.h"
#include"search.h"
#include"rangelayout.h"
#include"dataconversion.h"
#include<QMessageBox>
#include<math.h>
#include<codecvt>
#include<QTabWidget>
#include<QStandardItemModel>
#include"processinfo.h"
#include<Dbghelp.h>
#include<QFile>
#include"memoryviewer.h"
#include"cheats.h"
#include"lookup.h"
#include"conversionwindow.h"
#include"settings.h"
#include<QFileDialog>
#include<any>
#include <QStringList>
#include"pointersearch.h"
#include<QListWidgetItem>
#include"processlist.h"
#include"processselect.h"
#include"operativearray.h"

namespace Ui
{
    class MainWindow;
}

template<typename dType> struct AnyVal
{
    dType val;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    void readRanges();              //reads and lists all memory ranges
    void setup_search();            //prepares everything for value searches
    void setup_pointerSearch();
    void setup_dolphinInstance();   //creates an instance for searches on Dolphin
    void refresh_processList();     //reloads and recreates the process list
    void refresh_regionList();      //reloads and recreates the region list
    bool refresh_hook();            //recreates the hook to the process
    void setup_gameInfo();          //loads all useful information about the game
    void setup_specifications(int index);   //
    void listCheats();
    template<typename dType> void changePage(int page, int justify, bool BE);

    template<typename dType> void callSearch(int justify);
    //inline static Cheats& cheats = Cheats::getInstance();

    ~MainWindow();



private slots:
    void on_pushButton_hook_clicked();
    //void on_comboBox_value_type_currentIndexChanged(const QString &arg1);
    void on_pushButton_randomize_title_clicked();
    void on_pushButton_refresh_processList_clicked();
    void on_pushButton_refresh_regionList_clicked();
    void on_pushButton_test_clicked();
    //void on_comboBox_emulator_currentIndexChanged(int index);
    void on_comboBox_value_type_currentIndexChanged(int index);
    void on_pushButton_search_clicked();
    void on_comboBox_search_mode_currentIndexChanged(int index);
    void on_comboBox_searchCondition_currentIndexChanged(int index);
    void on_tableWidget_search_cellPressed(int row, int column);
    void on_pushButton_cancelSearch_clicked();
    void on_pushButton_poke_clicked();
    void on_checkBox_viewAsHex_clicked(bool checked);
    void on_spinBox_page_valueChanged(int arg1);
    void on_comboBox_ranges_search_currentIndexChanged(int index);
    void on_pushButton_open_MemViewer_clicked();
    void on_pushButton_executeCheats_clicked();
    void on_pushButton_dataConversion_clicked();
    void on_pushButton_saveSettings_clicked();
    void on_pushButton_setTempDir_clicked();
    void on_pushButton_setDefMemDumpDir_clicked();
    void on_pushButton_setCheatsDir_clicked();
    void on_pushButton_openCheatsDir_clicked();
    void on_pushButton_setDumpDir_clicked();
    void on_pushButton_dumpMemory_clicked();
    void on_pushButton_terminateCheats_clicked();
    void on_pushButton_openDump_clicked();
    void on_checkBox_BEPointers_clicked(bool checked);
    void on_checkBox_alignedPointers_clicked(bool checked);
    void on_pushButton_searchPointers_clicked();
    void on_comboBox_pointerDepth_currentIndexChanged(int index);
    void on_comboBox_theme_currentIndexChanged(const QString &arg1);
    void on_pushButton_saveTxtCheat_clicked();
    void on_listWidget_cheats_itemDoubleClicked(QListWidgetItem *item);
    void log();

private:
    Ui::MainWindow *ui;
    MemoryViewer* memoryviewer;
    ConversionWindow conversionWindow = ConversionWindow(this);
    Hook hook;                              //the hook to the process
   // Cheats cheats;
    std::any anyPokeVal;
    std::vector<MemRegion> memArr;          //contains information about all memory regions
    std::vector<RangeLayout> rangeMap;      //list of memory region layouts of the target process
    std::vector<ProcessInfo> processList;   //Contains all useful information about all running Processes
    MEMORY_BASIC_INFORMATION buffer = { 0 };//Read/Write buffer for memory information
    int typeSelect = 0;
    int selectedRange = 0;
    Search search;  //search instance
    int radix = 16;
    ProcessList* processListWND;

    char* pokeAddress;
    char pokeVal;
    QTimer* cheatTimer;
    QTimer* cheatTimerList;
    QStringList selectedLuaCheats;
    QStringList luaCheats;
    QDir luaCheatsDir;

    QString targetProcessName;  //target process name
    QString memDumpDir;
    int targetProcess;
    std::wstring currentProcess;
    DWORD currentPid;
    HANDLE currentHandle;
    PointerSearch pointerSearch;
    enum SearchType{unknown_ = 0, known_ = 1};
    template<typename dType> dType transferType(const dType type) { return type; }
    template<typename dType> inline void pokeRoutine(const dType val, char* addr);

    void setupSearchTab(bool val);
    void setupCheatsTab(bool val);
    void setupPointerTab(bool val);
    void setupProcessInfoTab(bool val);

    void* test(){ on_pushButton_terminateCheats_clicked(); }



public slots:
    void cancel();
    void printLuaLog();
    void clearLuaLog();
    void setProcessLineEdit(int index);
};




#endif // MAINWINDOW_H
