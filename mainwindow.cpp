#include "mainwindow.h"
#include "ui_mainwindow.h"

//Setting up UI and loading all settings
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    refresh_processList();

    ui->comboBox_emulator->addItem("Project64 (N64)", lookup::Project64_);
    ui->comboBox_emulator->addItem("Dolphin (GameCube, Wii, WiiWare)", lookup::Dolphin_);
    ui->comboBox_emulator->addItem("Cemu (Wii U)", lookup::Cemu_);
    ui->comboBox_emulator->addItem("VBA (GameBoy), experimental!", lookup::VBA_GB); //rename
    ui->comboBox_emulator->addItem("VBA (GameBoy Color), experimental!", lookup::VBA_GBC);
    ui->comboBox_emulator->addItem("VBA (GameBoy Advance)", lookup::VBA_GBA);
    ui->comboBox_emulator->addItem("DesMume (NDS)", lookup::DesMume_);
    ui->comboBox_emulator->addItem("Citra <Qt> 1468 (3DS), experimental!", lookup::Citra_);
    ui->comboBox_emulator->addItem("ePSXe (PSX)", lookup::EPSXE_);
    ui->comboBox_emulator->addItem("PCSX2 (PS2) experimental!", lookup::PCSX2_);
    ui->comboBox_emulator->addItem("RPCS3 (PS3), experimental!", lookup::RPCS3_);
    ui->comboBox_emulator->addItem("PPSSPP 1.9.3 <x64> (PSP), may work with later releases", lookup::ppsspp_);
    ui->comboBox_emulator->addItem("Fusion 3.64 (Master System)", lookup::FUSION_MM);
    ui->comboBox_emulator->addItem("Fusion 3.64 (Mega Drive/Genesis)", lookup::FUSION_MD);
    ui->comboBox_emulator->addItem("Fusion 3.64 (Sega CD), Experimental!", lookup::FUSION_CD);
    ui->comboBox_emulator->addItem("Fusion 3.64 (Game Gear)", lookup::FUSION_GG);
    ui->comboBox_emulator->addItem("Yabause v0.9.15 (Sega Saturn)", lookup::Yabause_);
    ui->comboBox_emulator->addItem("nullDC 1.0.4 (Sega Dreamcast)", lookup::nulldc_); //rename
    ui->comboBox_emulator->addItem("PC", lookup::PC_);


    //to be added
    //ui->comboBox_emulator->setItemData( 0, -1);
    //ui->comboBox_emulator->setItemData( 1, lookup::VirtuaNES_);
    //ui->comboBox_emulator->setItemData( 2, lookup::ZSNES9x_);
    //ui->comboBox_emulator->setItemData( 6, lookup::Yuzu_);

    ui->comboBox_search_mode->setItemData(0, unknown_);
    ui->comboBox_search_mode->setItemData(1, known_);


    {//scope where namespace "MungPlexSettings" is being used for better visibility
        using namespace MungPlexSettings;
        loadSettings();

        //load theme if not "Default"
        if(settingsObject["theme"].toString().compare("Default", Qt::CaseInsensitive) != 0)// compare == 0 -> is equal
        {
            QString x;
            QFile theme(QDir::currentPath() + "/css/" + settingsObject["theme"].toString() + ".css");
            if(!theme.open(QIODevice::ReadOnly)) { QMessageBox::warning(Q_NULLPTR, "Error loading theme", theme.errorString()); }
            QTextStream in(&theme);
            x.append(in.readAll());
            theme.close();
            MainWindow::setStyleSheet(x);
            ui->comboBox_theme->setCurrentText(settingsObject["theme"].toString());
        }

        //check if certain settings are present and correct
        if(!QDir(settingsObject["tempDir"].toString()).exists() ||
           !QDir(settingsObject["searchDumpDir"].toString()).exists() ||
           settingsObject["searchDumpDir"].isNull() || settingsObject["tempDir"].isNull())
        { QMessageBox::warning(this, "Warning", "\"Temp Directory\" is not set. Please set it before using this tool!"); }
        else{ ui->lineEdit_tempDir->setText(settingsObject["tempDir"].toString()); }

        if(!QDir(settingsObject["defaultMemDumpDir"].toString()).exists() || settingsObject["defaultMemDumpDir"].isNull())
        { QMessageBox::warning(this, "Warning", "\"Default Memory Dump Directory\" is not set. Please set it before using this tool!"); }
        else{ ui->lineEdit_defaultMemDumpDir->setText(settingsObject["defaultMemDumpDir"].toString()); }

        if(!QDir(settingsObject["cheatsDir"].toString()).exists() || settingsObject["defaultMemDumpDir"].isNull())
        { QMessageBox::warning(this, "Warning", "\"Cheats Directory\" is not set. Please set it before using this tool!"); }
        else{ ui->lineEdit_cheatsDir->setText(settingsObject["cheatsDir"].toString()); }
    }

    ui->comboBox_value_type->addItem("Int_8 / Byte", DataConversion::int8);
    ui->comboBox_value_type->addItem("Int_16 / 2 Bytes", DataConversion::int16);
    ui->comboBox_value_type->addItem("Int_32 / 4 Bytes", DataConversion::int32);
    ui->comboBox_value_type->addItem("Int_64 / 8 Bytes", DataConversion::int64);
    ui->comboBox_value_type->addItem("Float Single", DataConversion::float_);
    ui->comboBox_value_type->addItem("Float Double", DataConversion::double_);
    /*ui->comboBox_value_type->addItem("Int_8 Array", DataConversion::arrInt8);
    ui->comboBox_value_type->addItem("Int_16 Array", DataConversion::arrInt16);
    ui->comboBox_value_type->addItem("Int_32 Array", DataConversion::arrInt32);
    ui->comboBox_value_type->addItem("Int_64 Array", DataConversion::arrInt64);
    ui->comboBox_value_type->addItem("Float Single Array", DataConversion::arrFloat);
    ui->comboBox_value_type->addItem("Float Double Array", DataConversion::arrDouble);*/

    //make unimplemented ui elements invisible for release builds
    #ifdef QT_NO_DEBUG
        ui->pushButton_test->setEnabled(false);
        ui->pushButton_test->setVisible(false);
        ui->horizontalSlider_searchPrecision->setVisible(false);
        ui->checkBox_rollback_search->setVisible(false);
        ui->lineEdit_valExtension->setVisible(false);
        ui->label_valExtension->setVisible(false);
        ui->label_accuracy->setVisible(false);
        ui->label_searchPrecision->setVisible(false);
        ui->pushButton_randomize_title->setEnabled(false);
        ui->pushButton_randomize_title->setVisible(false);
    #endif

    this->setWindowTitle("MungPlex rev. " + QString::number(VER));

    cheatTimer = new QTimer(this);
    cheatTimerList = new QTimer(this);
    processListWND = new ProcessList(this);

    connect(cheatTimer, &QTimer::timeout, &Cheats::executeCheat);
    connect(cheatTimerList, &QTimer::timeout, &Cheats::executeCheatList);
    connect(&Cheats::getInstance(), &Cheats::error, this, &MainWindow::cancel);
    connect(&Cheats::getInstance(), &Cheats::log, this, &MainWindow::printLuaLog);
    connect(&Cheats::getInstance(), &Cheats::clear, this, &MainWindow::clearLuaLog);
    connect(processListWND, &ProcessList::doubleClicked, this, &MainWindow::setProcessLineEdit);
}


MainWindow::~MainWindow()
{
    delete cheatTimer;
    delete cheatTimerList;
    delete ui;
}

void MainWindow::log()
{
    on_pushButton_terminateCheats_clicked();
}

//hook up to the target emulator
void MainWindow::on_pushButton_hook_clicked()
{
    if(ui->comboBox_emulator->currentData() != lookup::PC_)
    {
        if(refresh_hook()) //if refresh was successfull...
        {
            setup_gameInfo();
            setup_search();
            setup_pointerSearch();

            ui->pushButton_open_MemViewer->setEnabled(true);
            ui->tableWidget_memoryRegions->setEnabled(true);
            ui->pushButton_refresh_regionList->setEnabled(true);

            setupSearchTab(true);
            setupCheatsTab(true);
            setupPointerTab(true);
        }
        ui->label_status->setText("Connected to " + QString::fromStdWString(currentProcess));
    }
    else
    {
        refresh_processList();
        processListWND->setProcessList(processList);
        processListWND->show();
    }
}

/* refreshes process list that stores information about all currently running processes.
 * This is used to get the target's emulator's PID and HANDLE
 * All entries will also be visible under the "System" tab
 */
void MainWindow::refresh_processList()
{
    this->processList.clear();
    ui->tableWidget_processList->removeRow(ui->tableWidget_processList->rowCount());

    HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0); //creates snapshot of avalaible processes
    PROCESSENTRY32W entry;                                         //entry point
    entry.dwSize = sizeof(entry);

    //do for each matching process name
    do
    {
        this->processList.push_back(ProcessInfo(entry.th32ProcessID, std::wstring(entry.szExeFile)));
    }while (Process32NextW(snap, &entry));

    ui->tableWidget_processList->setRowCount(static_cast<int>(this->processList.size()-1));

    QString arch;       //whether x86 or x64
    HANDLE handle_rep;  //represents current HANDLE
    for(int i = 0; i < static_cast<int>(this->processList.size()-1); i++)
    {
        ui->tableWidget_processList->setItem(i, 0, new QTableWidgetItem(QString::fromStdWString(this->processList[i+1].get_processName())));
        ui->tableWidget_processList->setItem(i, 1, new QTableWidgetItem(QString::number(this->processList[i+1].get_pid())));

        handle_rep = this->processList[i+1].get_handle();

        ui->tableWidget_processList->setItem(i, 2, new QTableWidgetItem(QString::number(reinterpret_cast<long long>(handle_rep))));

        if(this->processList[i+1].get_is_wowx64())
        {arch = "32-bit";}
        else{ arch = "64-bit"; }

        ui->tableWidget_processList->setItem(i, 3, new QTableWidgetItem(arch));

        ui->tableWidget_processList->item(i, 0)->setFlags(ui->tableWidget_processList->item(i, 0)->flags() ^ Qt::ItemIsEditable);
        ui->tableWidget_processList->item(i, 1)->setFlags(ui->tableWidget_processList->item(i, 1)->flags() ^ Qt::ItemIsEditable);
        ui->tableWidget_processList->item(i, 1)->setFlags(ui->tableWidget_processList->item(i, 2)->flags() ^ Qt::ItemIsEditable);
        ui->tableWidget_processList->item(i, 1)->setFlags(ui->tableWidget_processList->item(i, 3)->flags() ^ Qt::ItemIsEditable);
    }
}

/* refreshes region lists with information about all memory regions of the hooked emulator
 * This is used to find the emulated memory
 * All entries will also be visible under the "Process" tab
 */
void MainWindow::refresh_regionList()
{
    memArr.clear();
    ui->tableWidget_memoryRegions->removeRow(ui->tableWidget_processList->rowCount());
    ui->tableWidget_modules->removeRow(ui->tableWidget_modules->rowCount());

    DWORD pid_temp = hook.getPid();
    HANDLE hProcess_temp = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid_temp);
    hook.setHandle(hProcess_temp);
    MEMORY_BASIC_INFORMATION buffer = { 0 };

    unsigned long long rangeIterator = 0;

    unsigned long long addressSpace;
    if(hook.get_is_wowx64()) { addressSpace = 0xFF000000; } //Where to stop depending on architecture
    else{ addressSpace = 0x7FFFFF00000; } //0x7FFFFFF00000

    for(unsigned long long i = 0; (buffer.RegionSize + reinterpret_cast<unsigned long long>(buffer.BaseAddress)) < addressSpace; ++i)
    {
        VirtualQueryEx(hProcess_temp, reinterpret_cast<LPVOID>(rangeIterator), &buffer, sizeof(buffer));
        MemRegion temp_record(reinterpret_cast<unsigned long long>(buffer.BaseAddress), buffer.RegionSize, buffer.AllocationProtect, static_cast<int>(buffer.State), static_cast<int>(buffer.Type));
        memArr.push_back(temp_record);
        rangeIterator += buffer.RegionSize;
    }

    ui->tableWidget_memoryRegions->setRowCount(static_cast<int>(memArr.size()));

    for(int i = 0; i < static_cast<int>(memArr.size()); i++)
    {
        ui->tableWidget_memoryRegions->setItem(i, 0, new QTableWidgetItem(QString::number(memArr[i].get_address(), 16)));
        ui->tableWidget_memoryRegions->setItem(i, 1, new QTableWidgetItem(QString::number(memArr[i].get_size(), 16)));
        ui->tableWidget_memoryRegions->setItem(i, 2, new QTableWidgetItem(QString(memArr[i].is_mapped() ? "yes" : "no")));
        ui->tableWidget_memoryRegions->setItem(i, 3, new QTableWidgetItem((memArr[i].get_protectionStr())));
        ui->tableWidget_memoryRegions->item(i, 0)->setFlags(ui->tableWidget_memoryRegions->item(i, 0)->flags() ^ Qt::ItemIsEditable);
        ui->tableWidget_memoryRegions->item(i, 1)->setFlags(ui->tableWidget_memoryRegions->item(i, 1)->flags() ^ Qt::ItemIsEditable);
        ui->tableWidget_memoryRegions->item(i, 2)->setFlags(ui->tableWidget_memoryRegions->item(i, 2)->flags() ^ Qt::ItemIsEditable);
        ui->tableWidget_memoryRegions->item(i, 3)->setFlags(ui->tableWidget_memoryRegions->item(i, 3)->flags() ^ Qt::ItemIsEditable);
    }

    ProcessInfo processInfo(pid_temp, targetProcessName.toStdWString());
    std::vector<MODULE_PAIR> modules = processInfo.get_modulePairsInfo();

    ui->tableWidget_modules->setRowCount(modules.size());

    for(int i = 0; i < modules.size(); ++i)
    {
        ui->tableWidget_modules->setItem(i, 0, new QTableWidgetItem(QString::fromStdString(modules[i].first)));
        ui->tableWidget_modules->setItem(i, 1, new QTableWidgetItem(QString::number(modules[i].second, 16)));
        ui->tableWidget_modules->item(i, 0)->setFlags(ui->tableWidget_modules->item(i, 0)->flags() ^ Qt::ItemIsEditable);
        ui->tableWidget_modules->item(i, 1)->setFlags(ui->tableWidget_modules->item(i, 1)->flags() ^ Qt::ItemIsEditable);

    }
}

/* Randomizes Window Title
 * Planned just in case some PC game will detect this tool by looking up all processes
 * Implementation of PC game support isn't planned right now
 */
void MainWindow::on_pushButton_randomize_title_clicked()
{
    QString txt;

    for(int i = 0; i < 100; i++)
    {
        txt.append(QChar(rand() % (0x27bf - 0x2300 + 1) + 0x2300));
    }

    QWidget::setWindowTitle(txt);
    QMessageBox::information(this, "OK", "OK");
}

void MainWindow::on_pushButton_refresh_processList_clicked()
{
    refresh_processList();
}

void MainWindow::on_pushButton_refresh_regionList_clicked()
{
    refresh_regionList();
}

/* Refreshes the hook
 * This is necessary when selecting the target emulator or when another game has been launched
 */
bool MainWindow::refresh_hook()
{
    using namespace lookup;
    targetProcess = ui->comboBox_emulator->currentData().toInt();

    try
    {
        switch(targetProcess)   //Emulators are picked by their file name
        {
            case VirtuaNES_: currentProcess = L"VirtuaNES"; break;
            case Project64_: currentProcess = L"Project64"; break;
            case Dolphin_: currentProcess = L"Dolphin"; break;
            case Cemu_: currentProcess = L"Cemu"; break;
            case VBA_GB: case VBA_GBC: case VBA_GBA: currentProcess = L"VBA.exe";  qDebug() << "VBA selected"; break;
            case DesMume_: currentProcess = L"DeSmuME"; qDebug() << "DeSmuME selected"; break;
            case PCSX2_: currentProcess = L"pcsx2"; qDebug() << "PCSX2.exe selected"; break;
            case EPSXE_: currentProcess = L"ePSXe"; qDebug() << "ePSXe.exe selected"; break;
            case RPCS3_: currentProcess = L"rpcs3"; qDebug() << "rpcs3.exe selected"; break;
            case FUSION_MM: case FUSION_GG: case FUSION_MD: case FUSION_CD: currentProcess = L"Fusion";  qDebug() << "Fusion selected"; break;
            case Yabause_: currentProcess = L"yabause"; qDebug() << "yabause.exe selected"; break;
            case nulldc_: currentProcess = L"nullDC"; qDebug() << "nullDC.exe selected"; break;
            case ppsspp_: currentProcess = L"PPSSPPWindows64"; qDebug() << "PPSSPPWindows64.exe selected"; break;
            case Citra_: currentProcess = L"citra-qt"; qDebug() << "citra-qt.exe selected"; break;
            case PC_: currentProcess = ui->lineEdit_processName->text().toStdWString(); qDebug() << currentProcess << " selected"; break;
        default: return false;
        }

        //browses process list to find the target emulator
        refresh_processList();
        for(unsigned long long i = 0; i < processList.size(); i++)
        {
            QString pName = QString::fromStdWString(processList[i].get_processName());
            QString cName = QString::fromStdWString(currentProcess);

            if(pName.contains(cName, Qt::CaseInsensitive))
            {
                hook = Hook(processList[i].get_pid(), processList[i].get_handle(), reinterpret_cast<LPCSTR>(processList[i].get_processName().c_str()));
                hook.set_is_wowx64(processList[i].get_is_wowx64());
                refresh_regionList();
                rangeMap.clear();
                HANDLE asdf = OpenProcess(PROCESS_ALL_ACCESS, NULL, hook.getPid());
                hook.setHandle(asdf);
                ProcessSelect::getInstance().initialize(hook, memArr, targetProcess);
                rangeMap = ProcessSelect::getInstance().getRangeMap();
                return true;
            }
        }
    }
    catch(std::exception e)
    {
        qDebug() << e.what();
    }
    return false;
}

//button for testing
void MainWindow::on_pushButton_test_clicked()
{

}

//fetches game specific information
void MainWindow::setup_gameInfo()
{
    ui->tableWidget_gameInfo->setColumnCount(1);
    ui->tableWidget_gameInfo->setRowCount(ProcessSelect::getInstance().getInfoData()->count());
    ui->tableWidget_gameInfo->setVerticalHeaderLabels(*ProcessSelect::getInstance().getInfoHeader());
    for(int i = 0; i < ProcessSelect::getInstance().getInfoData()->count(); ++i)
    { ui->tableWidget_gameInfo->setItem(i, 0, new QTableWidgetItem(ProcessSelect::getInstance().getInfoData()->at(i))); }
}

//prepares memory search
void MainWindow::setup_search()
{
    ui->checkBox_BE->setChecked(ProcessSelect::getInstance().isBE());
    ui->comboBox_ranges_search->clear();
    QStringList tempList;

    for(int i = 0; i < this->rangeMap.size(); i++)
    {
        tempList.append((*(this->rangeMap[i].get_lable()) + ": 0x" + QString::number(this->rangeMap[i].get_virtualAddress(), 16)));
    }

    ui->lineEdit_rangeStart_info->setText(QString::number((rangeMap[0].get_virtualAddress()), 16));

    if(rangeMap[0].get_size() > 0x10000000)
    { ui->lineEdit_rangeEnd_info->setText(QString::number((rangeMap[0].get_virtualAddress() + 0x1FFFFFFF), 16).toUpper()); }
    else{ ui->lineEdit_rangeEnd_info->setText(QString::number((rangeMap[0].get_virtualAddress() + rangeMap[0].get_size() -1), 16).toUpper()); }

    ui->comboBox_ranges_search->addItems(tempList);
    return;
}

//copies range select to the pointer search tab
void MainWindow::setup_pointerSearch()
{
    ui->comboBox_selectRangeToDump->clear();

    for(unsigned long long i = 0; i < rangeMap.size(); ++i)
    {
        QString mlem = (*rangeMap[i].get_lable() + ": 0x" + QString::number(rangeMap[i].get_virtualAddress(), 16));
        ui->comboBox_selectRangeToDump->addItem(mlem);
    }
}

void MainWindow::on_comboBox_value_type_currentIndexChanged(int index)
{
    typeSelect = ui->comboBox_value_type->itemData(index).value<int>();
}

//defines data types for current cheat search
void MainWindow::on_pushButton_search_clicked()
{
    bool isSigned = ui->checkBox_signed->isChecked();
    int searchMode = ui->comboBox_search_mode->currentIndex();

    switch(typeSelect)
    {
    case DataConversion::int8: isSigned ? callSearch<char>(2) : callSearch<unsigned char>(2); break;
    case DataConversion::int16: isSigned ? callSearch<short>(4) : callSearch<unsigned short>(4); break;
    case DataConversion::int32: isSigned ? callSearch<int>(8) : callSearch<unsigned int>(8); break;
    case DataConversion::int64: isSigned ? callSearch<long long>(16) : callSearch<unsigned long long>(16); break;
    case DataConversion::float_: callSearch<float>(0); break;
    case DataConversion::double_: callSearch<double>(0); break;
    /*case DataConversion::arrInt8: isSigned ? callSearch<OperativeArray<char>>(0) : callSearch<OperativeArray<unsigned char>>(0); break;
    case DataConversion::arrInt16: isSigned ? callSearch<OperativeArray<short>>(0) : callSearch<OperativeArray<unsigned short>>(0); break;
    case DataConversion::arrInt32: isSigned ? callSearch<OperativeArray<int>>(0) : callSearch<OperativeArray<unsigned int>>(0); break;
    case DataConversion::arrInt64: isSigned ? callSearch<OperativeArray<long long>>(0) : callSearch<OperativeArray<unsigned long long>>(0); break;
    case DataConversion::arrFloat: callSearch<OperativeArray<float>>(0); break;
    case DataConversion::arrDouble: callSearch<OperativeArray<double>>(0); break;*/
    }
}

//calls memory search with all user-defined settings
template<typename dType> void MainWindow::callSearch(int justify)
{
    bool isSigned = ui->checkBox_signed->isChecked();
    bool isKnown = false;
    bool BE = ui->checkBox_BE->isChecked();
    bool isAligned = ui->checkBox_align4->isChecked();
    unsigned long long resultCount;
    unsigned long long alignment = ((isAligned) ? ((sizeof(dType) == 2) ? 2 : 4) : 1 );

    if(ui->comboBox_search_mode->currentIndex() > 0) { isKnown = true; }

    if(search.isFirstSearch()) //if first search
    {
        //get range limitations
        selectedRange = ui->comboBox_ranges_search->currentIndex();
        unsigned long long vStart = ui->lineEdit_rangeStart_info->text().toULongLong(Q_NULLPTR, 16);
        unsigned long long vEnd = ui->lineEdit_rangeEnd_info->text().toULongLong(Q_NULLPTR, 16);
        unsigned long long sSize = vEnd - vStart;
        unsigned long long pStart = rangeMap[selectedRange].get_address() + vStart - rangeMap[selectedRange].get_virtualAddress();

        //setup search instance
        search.setSearchRange(vStart, vEnd, pStart);
        search.set_alignment(alignment);
        ui->pushButton_cancelSearch->setEnabled(true);
        search.set_hook(&hook);
        selectedRange = ui->comboBox_ranges_search->currentIndex();

        ui->pushButton_search->setText("Next");
        search.incIteration();
    }
    else { search.incIteration(); }

    //calls search method with all parameters
    switch(typeSelect)
    {
        case DataConversion::float_: search.search<float>(isKnown, BE, ui->lineEdit_knownValue->text().toFloat(Q_NULLPTR)); break;
        case DataConversion::double_: search.search<double>(isKnown, BE, ui->lineEdit_knownValue->text().toDouble(Q_NULLPTR)); break;




        default:
            if(isSigned){ search.search<dType>(isKnown, BE, static_cast<dType>(ui->lineEdit_knownValue->text().toLongLong(Q_NULLPTR, radix))); }
            else{ search.search<dType>(isKnown, BE, static_cast<dType>(ui->lineEdit_knownValue->text().toULongLong(Q_NULLPTR, radix))); }
        break;
    }

    resultCount = search.get_hitsCount();

    //computes amount of result pages
    unsigned long long x = resultCount%1024;
    if(x != 0) { x = resultCount/1024 +1; }
    else { x = resultCount/1024; }
    ui->label_totalPages->setText(QString::number(x));

    if(search.isFirstSearch()){ search.firstSearchDone(false); }

    //opens first page, prints results and iteration count
    changePage<dType>(1, justify, BE);
    ui->label_results->setText(QString::number(resultCount, 10));
    ui->label_iterations->setText(QString::number(search.getIterations(), 10));

    //if results are present unlock poke options
    if(resultCount > 0)
    {
        ui->lineEdit_pokeAddress->setEnabled(true);
        ui->lineEdit_pokeValue->setEnabled(true);
        ui->pushButton_poke->setEnabled(true);
        ui->spinBox_page->setEnabled(true);
    }
}

//enables known value search
void MainWindow::on_comboBox_search_mode_currentIndexChanged(int index)
{
    ui->lineEdit_knownValue->setEnabled(index > 0 ? true : false);
}

//sets search condition
void MainWindow::on_comboBox_searchCondition_currentIndexChanged(int index)
{
    search.set_condition(index);
}

//moves row values to the poke area by clicking the corresponding row
void MainWindow::on_tableWidget_search_cellPressed(int row, int column)
{
    pokeAddress = reinterpret_cast<char*>(ui->tableWidget_search->item(row, 0)->text().toLongLong(Q_NULLPTR, 16));
    QString val = ui->tableWidget_search->item(row, (ui->checkBox_pokePrevious->isChecked() ? 2 : 1))->text();
    //qDebug() << ui->tableWidget_search->item(row, 1)->text().toInt(Q_NULLPTR, radix);
    ui->lineEdit_pokeAddress->setText(QString::number(reinterpret_cast<long long>(pokeAddress), 16));
    ui->lineEdit_pokeValue->setText(val);
}

//Cancels search
void MainWindow::on_pushButton_cancelSearch_clicked()
{
    ui->pushButton_search->setText("New Search");
    ui->tableWidget_search->clear();
    ui->tableWidget_search->setHorizontalHeaderLabels(QStringList() << "Address" << "Current" << "Previous" << "Difference");
    ui->pushButton_cancelSearch->setEnabled(false);
    search.cancel();
}

//pokes single value or all selected into memory
void MainWindow::on_pushButton_poke_clicked()
{
    if(ui->checkBox_pokeSelected->isChecked())
    {
        QList list = ui->tableWidget_search->selectedItems();
        QList<QString> addrList;
        QList<QString> valList;

        for(int i = 0; i < list.count(); ++i)
        {
            if(list[i]->column() == 0)
            {
                addrList.push_back(list.value(i)->text());
                valList.push_back((ui->checkBox_pokePrevious->isChecked() ? list.value(i+2)->text() : ui->lineEdit_pokeValue->text()));
            }
        }

        for(int i = 0; i < addrList.size(); ++i)
        {
            char* addr = reinterpret_cast<char*>(addrList[i].toLongLong(Q_NULLPTR, 16));

            switch(typeSelect)
            {
            case DataConversion::int8:
                pokeRoutine<char>(static_cast<char>(valList[i].toInt(Q_NULLPTR, radix)), addr);
            break;
            case DataConversion::int16:
                pokeRoutine<short>(static_cast<short>(valList[i].toShort(Q_NULLPTR, radix)), addr);
            break;
            case DataConversion::int32:
                pokeRoutine<int>(static_cast<int>(valList[i].toInt(Q_NULLPTR, radix)), addr);
            break;
            case DataConversion::int64:
                pokeRoutine<long long>(static_cast<long long>(valList[i].toLongLong(Q_NULLPTR, radix)), addr);
            break;

            case DataConversion::float_:
                pokeRoutine<float>(static_cast<float>(valList[i].toFloat(Q_NULLPTR)), addr);
            break;
            case DataConversion::double_:
                pokeRoutine<double>(static_cast<double>(valList[i].toDouble(Q_NULLPTR)), addr);
            break;
            }
        }
    }
    else //single value poke
    {
        char* addr = reinterpret_cast<char*>(ui->lineEdit_pokeAddress->text().toLongLong(Q_NULLPTR, 16));
        switch(typeSelect)
        {
        case DataConversion::int8:
            pokeRoutine<char>(static_cast<char>(ui->lineEdit_pokeValue->text().toInt(Q_NULLPTR, radix)), addr);
        break;
        case DataConversion::int16:
            pokeRoutine<short>(static_cast<short>(ui->lineEdit_pokeValue->text().toShort(Q_NULLPTR, radix)), addr);
        break;
        case DataConversion::int32:
            pokeRoutine<int>(static_cast<int>(ui->lineEdit_pokeValue->text().toInt(Q_NULLPTR, radix)), addr);
        break;
        case DataConversion::int64:
            pokeRoutine<long long>(static_cast<long long>(ui->lineEdit_pokeValue->text().toLongLong(Q_NULLPTR, radix)), addr);
        break;

        case DataConversion::float_:
            pokeRoutine<float>(static_cast<float>(ui->lineEdit_pokeValue->text().toFloat(Q_NULLPTR)), addr);
        break;
        case DataConversion::double_:
            pokeRoutine<double>(static_cast<double>(ui->lineEdit_pokeValue->text().toDouble(Q_NULLPTR)), addr);
        break;
        }
    }
}

//calls poke methode
template<typename dType> inline void MainWindow::pokeRoutine(const dType val, char* addr)
{
    bool isBE = ui->checkBox_BE->isChecked();
    search.poke<dType>(addr, isBE ? DataConversion::swapBytes<dType>(val) : val);
}

//whether to display results as dec or hex
void MainWindow::on_checkBox_viewAsHex_clicked(bool checked)
{
    if(checked) { radix = 16; }
    else{radix = 10; }
}

//changes to the required pages
void MainWindow::on_spinBox_page_valueChanged(int val)
{
    bool isSigned = ui->checkBox_signed->isChecked();
    bool BE = ui->checkBox_BE->isChecked();

    switch(typeSelect)
    {
    case DataConversion::int8: isSigned ? changePage<char>(val, 2, false) : changePage<unsigned char>(val, 2, false); break;
    case DataConversion::int16: isSigned ? changePage<short>(val, 4, BE) : changePage<unsigned short>(val, 4, BE); break;
    case DataConversion::int32: isSigned ? changePage<int>(val, 8, BE) : changePage<unsigned int>(val, 8, BE); break;
    case DataConversion::int64: isSigned ? changePage<long long>(val, 16, BE) : changePage<unsigned long long>(val, 16, BE); break;
    case DataConversion::float_: changePage<float>(val, 0, BE); break;
    case DataConversion::double_: changePage<double>(val, 0, BE); break;
    }
}

//updates page
template<typename dType>void MainWindow::changePage(int page, int justify, bool BE)
{

    int displayRows = 1024;
    int hits = search.get_hitsCount();
    int pages = hits / 1024;
    int rest = hits % 1024;
    if(rest != 0){ ++pages; }
    ui->tableWidget_search->clear();
    ui->tableWidget_search->setHorizontalHeaderLabels(QStringList() << "Address" << "Current" << "Previous" << "Difference");
    ui->tableWidget_search->setRowCount(1024);

    //ensures the right amount of rows will be drawn to prevent crashes
    if((page == 1 || page == 0) && hits < 1024)
    {
        ui->tableWidget_search->setRowCount(hits);
        displayRows = hits;
    }
    else if(page == pages && rest > 0)
    {
        ui->tableWidget_search->setRowCount(rest);
        displayRows = rest;
    }

    ui->spinBox_page->setMaximum(pages); //prevents from attempting to open a nin-existent page

    //draws table rows depending on whether results should be viewed as hex or dec
    if(radix == 16)
    {
        //used to remove unneccessary leading Fs
        long long cleaner = -1;
        switch(sizeof(dType))
        {
            case 1: cleaner = 0xFF; break;
            case 2: cleaner = 0xFFFF; break;
            case 4: cleaner = 0xFFFFFFFF; break;
        }

        for(int i = 0; i < displayRows; i++)
        {
            ui->tableWidget_search->setItem(i, 0, new QTableWidgetItem(QString::number(search.get_virtualStartAddress() + *(search.get_offsetList()+i+((page-1)*displayRows)), 16).rightJustified(8, '0')));

            dType tVal = *reinterpret_cast<dType*>((search.get_dumpAddress() + *((search.get_offsetList()+i+((page-1)*displayRows)))));
            tVal = (BE) ? DataConversion::swapBytes(tVal) : tVal;
            ui->tableWidget_search->setItem(i, 1, new QTableWidgetItem(QString::number(static_cast<long long>(tVal) & cleaner, radix).rightJustified(justify, '0'))); //

            dType tValOld = *reinterpret_cast<dType*>((search.get_oldDumpAddress() + *((search.get_offsetList()+i+((page-1)*displayRows)))));
            tValOld = (BE) ? DataConversion::swapBytes(tValOld) : tValOld;
            ui->tableWidget_search->setItem(i, 2, new QTableWidgetItem(QString::number(static_cast<long long>(tValOld) & cleaner, radix).rightJustified(justify, '0'))); //

            tVal = (tVal == tValOld) ? 0 : ((tVal > tValOld) ? (tVal - tValOld) : (tValOld - tVal));
            ui->tableWidget_search->setItem(i, 3, new QTableWidgetItem(QString::number(static_cast<long long>(tVal) & cleaner, radix).rightJustified(justify, '0'))); //
        }
    }
    else
    {
        for(int i = 0; i < displayRows; i++)
        {
            ui->tableWidget_search->setItem(i, 0, new QTableWidgetItem(QString::number(search.get_virtualStartAddress() + *(search.get_offsetList()+i+((page-1)*displayRows)), 16).rightJustified(8, '0')));

            dType tVal = *reinterpret_cast<dType*>((search.get_dumpAddress() + *((search.get_offsetList()+i+((page-1)*displayRows)))));
            tVal = (BE) ? DataConversion::swapBytes(tVal) : tVal;
            ui->tableWidget_search->setItem(i, 1, new QTableWidgetItem(QString::number(static_cast<dType>(tVal)))); //

            dType tValOld = *reinterpret_cast<dType*>((search.get_oldDumpAddress() + *((search.get_offsetList()+i+((page-1)*displayRows)))));
            tValOld = (BE) ? DataConversion::swapBytes(tValOld) : tValOld;
            ui->tableWidget_search->setItem(i, 2, new QTableWidgetItem(QString::number(static_cast<dType>(tValOld)))); //

            tVal = (tVal == tValOld) ? 0 : ((tVal > tValOld) ? (tVal - tValOld) : (tValOld - tVal));
            ui->tableWidget_search->setItem(i, 3, new QTableWidgetItem(QString::number(static_cast<dType>(tVal)))); //
        }
    }
}

void MainWindow::on_comboBox_ranges_search_currentIndexChanged(int index)
{
    //if((ui->comboBox_ranges_search->count() > 1) && (index == 0)) //provisional avoidance of invalid selection
    //{ ui->comboBox_ranges_search->setCurrentIndex(1); }
    ui->lineEdit_rangeStart_info->setText(QString::number(rangeMap[index].get_virtualAddress(), 16));
    ui->lineEdit_rangeEnd_info->setText(QString::number(rangeMap[index].get_virtualAddress() + rangeMap[index].get_size()-1, 16));
}

//opens memory viewer
void MainWindow::on_pushButton_open_MemViewer_clicked()
{
    memoryviewer = new MemoryViewer(this);
    memoryviewer->setHook(hook);
    memoryviewer->setBE(ProcessSelect::getInstance().isBE());
    memoryviewer->set_rangeLayouts(rangeMap);
    memoryviewer->set_rangeComboBox();
    memoryviewer->readMemory(reinterpret_cast<char*>(rangeMap[0].get_address()));
    memoryviewer->setOffset(0, rangeMap[0].get_virtualAddress());
    memoryviewer->show();
}

//executes cheats from list or textEdit depending on selection
void MainWindow::on_pushButton_executeCheats_clicked()
{
    ui->plainTextEdit_luaLog->clear();
    Cheats::getInstance().set_rangeLayouts(rangeMap);
    Cheats::getInstance().setHook(hook);
    Cheats::getInstance().processSnapshot();

    if(ui->radioButton_luaText->isChecked())
    {
        ui->plainTextEdit_luaLog->appendPlainText("Executing text-cheat");
        std::string val = ui->plainTextEdit_luaCheat->toPlainText().toStdString();
        Cheats::getInstance().setCheat(val);
        Cheats::getInstance().sendLuaCheat();
        cheatTimer->start(8);
    }
    else
    {
        ui->plainTextEdit_luaLog->appendPlainText("Executing cheats from list:");

        selectedLuaCheats.clear();
        Cheats::getInstance().sendLuaCheat();


        for(struct{int i = 0; int c = 0;} mlem; mlem.i < ui->listWidget_cheats->count(); ++mlem.i)
        {
            if(ui->listWidget_cheats->item(mlem.i)->checkState() == Qt::Checked)
            {
                ++mlem.c;
                selectedLuaCheats.append(luaCheats.at(mlem.i));
                ui->plainTextEdit_luaLog->appendPlainText(QString::number(mlem.c) + ": " + ui->listWidget_cheats->item(mlem.i)->text());
            }
        }

        Cheats::getInstance().setupCheatList(selectedLuaCheats, luaCheatsDir);
        Cheats::getInstance().loadCheatsFromDisk();
        cheatTimerList->start(8);
    }

    ui->pushButton_terminateCheats->setEnabled(true);
    ui->pushButton_executeCheats->setEnabled(false);
}

//Terminates cheats
void MainWindow::on_pushButton_terminateCheats_clicked()
{
    cheatTimer->stop();
    cheatTimerList->stop();
    ui->pushButton_terminateCheats->setEnabled(false);
    ui->pushButton_executeCheats->setEnabled(true);
}

//Opens Data Conversion window
void MainWindow::on_pushButton_dataConversion_clicked()
{
    conversionWindow.show();
}

//saves settings
void MainWindow::on_pushButton_saveSettings_clicked()
{
    QString x = ui->comboBox_theme->currentText();
    MungPlexSettings::updateSettings("theme", x);
    MungPlexSettings::saveSettings();
}

//sets directory for temp data
void MainWindow::on_pushButton_setTempDir_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Choose Directory"), "/home", QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    MungPlexSettings::updateSettings("tempDir", dir);
    ui->lineEdit_tempDir->clear();
    ui->lineEdit_tempDir->setText(dir);
    if(!QDir(dir.append("/SearchDumps")).exists()){ QDir().mkdir(dir); }
    MungPlexSettings::updateSettings("searchDumpDir", dir);
}

//sets default mem dump directory
void MainWindow::on_pushButton_setDefMemDumpDir_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Choose Directory"), "/home", QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    MungPlexSettings::updateSettings("defaultMemDumpDir", dir);
    ui->lineEdit_defaultMemDumpDir->clear();
    ui->lineEdit_defaultMemDumpDir->setText(dir);
}

//sets theme
void MainWindow::on_comboBox_theme_currentIndexChanged(const QString &arg1)
{
    MungPlexSettings::updateSettings("theme", arg1);
}

//sets cheat directories
void MainWindow::on_pushButton_setCheatsDir_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Choose Directory"), "/home", QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    MungPlexSettings::updateSettings("cheatsDir", dir);
    ui->lineEdit_cheatsDir->clear();
    ui->lineEdit_cheatsDir->setText(dir);
    //if(!QDir(dir.append("/Lua")).exists()){ QDir().mkdir(dir); }
    if(!QDir(dir + "/GameCube").exists()){ QDir().mkdir(dir + "/GameCube"); }
    if(!QDir(dir + "/Wii").exists()){ QDir().mkdir(dir + "/Wii"); }
    if(!QDir(dir + "/WiiWare").exists()){ QDir().mkdir(dir + "/WiiWare"); }
    if(!QDir(dir + "/Wii_U").exists()){ QDir().mkdir(dir + "/Wii_U"); }
    if(!QDir(dir + "/GameBoy").exists()){ QDir().mkdir(dir + "/GameBoy"); }
    if(!QDir(dir + "/GameBoy_Color").exists()){ QDir().mkdir(dir + "/GameBoy_Color"); }
    if(!QDir(dir + "/GameBoy_Advance").exists()){ QDir().mkdir(dir + "/GameBoy_Advance"); }
    if(!QDir(dir + "/Nintendo_DS").exists()){ QDir().mkdir(dir + "/Nintendo_DS"); }
}

//opens cheat directory
void MainWindow::on_pushButton_openCheatsDir_clicked()
{
    luaCheatsDir.setPath(MungPlexSettings::settingsObject["cheatsDir"].toString());
    luaCheatsDir.setPath(QFileDialog::getExistingDirectory(this, tr("Choose Directory"), luaCheatsDir.path(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks));
    listCheats();
}

//lists cheats in UI
void MainWindow::listCheats()
{
    ui->listWidget_cheats->clear();
    luaCheats.clear();
    luaCheats = luaCheatsDir.entryList(QStringList() << "*.lua" << "*.LUA" << "*.Lua", QDir::Files);
    foreach(QString cheat, luaCheats)
    {
        QListWidgetItem* item = new QListWidgetItem(cheat);
        item->setFlags(item->flags() | (Qt::ItemIsUserCheckable));
        item->setCheckState(Qt::Unchecked);
        ui->listWidget_cheats->addItem(item);
    }

    if(luaCheats.count())
    {
        ui->radioButton_luaText->setChecked(false);
        ui->radioButton_LuaFiles->setEnabled(true);
        ui->radioButton_LuaFiles->setChecked(true);
        ui->listWidget_cheats->setEnabled(true);
    }
}

//saves text cheat as a new .lua file and refreshes list on UI
void MainWindow::on_pushButton_saveTxtCheat_clicked()
{
    QString cheat = ui->plainTextEdit_luaCheat->toPlainText();
    luaCheatsDir.setPath(MungPlexSettings::settingsObject["cheatsDir"].toString());
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save Cheat as..."), luaCheatsDir.path(), tr("Lua script file (*.lua)"));
    QFile file(fileName);
    QStringList chunks(fileName.split("/")); //fileName also contains the actual file in url
    chunks.pop_back(); //removes actual file from url

    //rebuild fileName string
    fileName.clear();
    for(int i = 0; i < chunks.count(); ++i)
    {
        fileName.append(chunks.at(i) + "/");
    }
    fileName.chop(1); //remove slash at the end
    luaCheatsDir.setPath(fileName);

    if(!file.open(QFile::WriteOnly | QFile::Text)){ qDebug() << "could not save cheat"; return;  }
    QTextStream out (&file);
    out.setEncoding(QStringConverter::Utf8);
    out << cheat;
    file.flush();
    file.close();

    listCheats(); //list all cheats again, now including the newly added one
}

//sets dump directory for pointer searches
void MainWindow::on_pushButton_setDumpDir_clicked()
{
    memDumpDir.clear();
    if(ui->lineEdit_dumpFileName->text().length() > 0)
    {
        memDumpDir = QFileDialog::getExistingDirectory(this, tr("Choose Directory"), MungPlexSettings::settings["defaultMemDumpDir"].toString(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
        memDumpDir.append("/" + ui->lineEdit_dumpFileName->text());
        ui->pushButton_dumpMemory->setEnabled(true);
        ui->lineEdit_dumpDir->setText(memDumpDir);
    }
    else
    {
        QMessageBox::warning(this, "Warning", "Enter a valid file name, please.");
    }
}

//dumps memory range
void MainWindow::on_pushButton_dumpMemory_clicked()
{
    pointerSearch.setHook(hook);
    pointerSearch.setPath(memDumpDir);
    pointerSearch.setRangeLayout(rangeMap);
    pointerSearch.setSelectedRange(static_cast<unsigned long long>(ui->comboBox_selectRangeToDump->currentIndex()));
    pointerSearch.readRange();
    pointerSearch.dump();
}

//opens mem dump for pointer search
void MainWindow::on_pushButton_openDump_clicked()
{
    memDumpDir.clear();
    memDumpDir = QFileDialog::getOpenFileName(this, tr("Choose Dump File"), MungPlexSettings::settings["defaultMemDumpDir"].toString(), tr("Dump Files (*.bin *.img *.raw *.dat) ;; All files (*.*)"));
    pointerSearch.setHook(hook);
    pointerSearch.setPath(memDumpDir);
    pointerSearch.load();
    if(memDumpDir.length()){ ui->pushButton_searchPointers->setEnabled(true); }
}

//sets BE true if checked
void MainWindow::on_checkBox_BEPointers_clicked(bool checked)
{
    pointerSearch.setBE(checked);
}

//sets alignment true if checked
void MainWindow::on_checkBox_alignedPointers_clicked(bool checked)
{
    pointerSearch.setAligned(checked);
}

//searches for pointers
void MainWindow::on_pushButton_searchPointers_clicked()
{
    ui->plainTextEdit_pointerResults->clear();

    //fetches parameters
    unsigned long long resultCount = 0;
    unsigned long long baseAddress = ui->lineEdit_baseAddress->text().toULongLong(Q_NULLPTR, 16);
    unsigned long long maxOffset = ui->lineEdit_maxPointerOffset->text().toULongLong(Q_NULLPTR, 16);
    long long temp = ui->lineEdit_minPointerOffset->text().toLongLong(Q_NULLPTR, 16);
    unsigned long long minOffset = static_cast<unsigned long long>(abs(temp));
    unsigned long long pointerDestination = ui->lineEdit_pointerDestination->text().toULongLong(Q_NULLPTR, 16);
    pointerSearch.setSearchInfo(baseAddress, maxOffset, minOffset, pointerDestination);

    resultCount = pointerSearch.search(); //calls search methode

    ui->label_pointerResults->setText(QString::number(resultCount));

    //fetches results
    if(resultCount <= 512)
    {
        if(ui->comboBox_pointerDepth->currentIndex() == 0) //if level == 1
        {
            unsigned int* dumpAddress = reinterpret_cast<unsigned int*>(pointerSearch.getDumpAddress());
            int* resultOffsets = pointerSearch.getOffsetList();
            unsigned long long* resultsAddress = pointerSearch.getResultsAddress();

            for(unsigned long long i = 0; i < resultCount; ++i)
            {   unsigned int nthResult = *(resultsAddress + i);
                unsigned long long ptr = (*(dumpAddress + nthResult/4));
                unsigned long long offset = *(resultOffsets + i);//pointerDestination - ptr;
                unsigned long long val = *(dumpAddress + (pointerDestination - baseAddress)/4);
                ui->plainTextEdit_pointerResults->appendPlainText("[0x" + QString::number(nthResult + baseAddress, 16) + "] " +
                                                    (((int)offset>=0) ? "+ 0x" + QString::number(offset, 16) : "- 0x" + QString::number(-offset, 16)) +
                                                    " = 0x" + QString::number(pointerDestination, 16));
            }
        }
        else //level == 2
        {
            unsigned int* dumpAddress = reinterpret_cast<unsigned int*>(pointerSearch.getDumpAddress());
            int* resultOffsets = pointerSearch.getOffsetList();
            unsigned long long* resultsAddress = pointerSearch.getResultsAddress();

            for(unsigned long long i = 0; i < resultCount*2; i+=2)
            {   unsigned int nthResult = *(resultsAddress + i/2);
                unsigned long long ptr = (*(dumpAddress + nthResult/4));
                unsigned long long offset = *(resultOffsets + i);//pointerDestination - ptr;
                unsigned long long offset2 = *(resultOffsets + i+1);
                unsigned long long val = *(dumpAddress + (pointerDestination - baseAddress)/4);
                ui->plainTextEdit_pointerResults->appendPlainText("[[0x" + QString::number(nthResult + baseAddress, 16) + "] "
                                                    + (((int)offset>=0) ? "+ 0x" + QString::number(offset, 16) : "- 0x" + QString::number(-offset, 16))
                                                    + "] " + (((int)offset2>=0) ? "+ 0x" + QString::number(offset2, 16) : "- 0x" + QString::number(-offset2, 16))
                                                    + " = 0x" + QString::number(pointerDestination, 16));
            }
            //qDebug() << "resultsOffsets: " << resultOffsets;
        }
    }
    else{ ui->plainTextEdit_pointerResults->appendPlainText("Too many results to display.\nPlease refine..."); }
}

//selects pointer depth
void MainWindow::on_comboBox_pointerDepth_currentIndexChanged(int index)
{
    pointerSearch.setPointerLevel(index+1);
}

//enables widgets
void MainWindow::setupSearchTab(bool val)
{
    ui->comboBox_ranges_search->setEnabled(val);
    ui->lineEdit_rangeStart_info->setEnabled(val);
    ui->lineEdit_rangeEnd_info->setEnabled(val);
    ui->comboBox_value_type->setEnabled(val);
    ui->checkBox_BE->setEnabled(val);
    ui->checkBox_signed->setEnabled(val);
    ui->checkBox_align4->setEnabled(val);
    ui->comboBox_search_mode->setEnabled(val);
    ui->comboBox_searchCondition->setEnabled(val);
    //ui->lineEdit_knownValue->setEnabled(val);
    ui->pushButton_search->setEnabled(val);
    ui->checkBox_viewAsHex->setEnabled(val);
    ui->checkBox_pokePrevious->setEnabled(val);
    ui->checkBox_pokeSelected->setEnabled(val);

    //to be implemented later
    //ui->horizontalSlider_searchPrecision->setEnabled(val);
    //ui->checkBox_rollback_search->setEnabled(val);
    //ui->lineEdit_valExtension->setEnabled(val);
}

//enables widgets
void MainWindow::setupCheatsTab(bool val)
{
    ui->pushButton_openCheatsDir->setEnabled(val);
    ui->plainTextEdit_luaCheat->setEnabled(val);
    ui->pushButton_executeCheats->setEnabled(val);
    ui->radioButton_luaText->setEnabled(val);
    ui->radioButton_luaText->setChecked(val);
    ui->radioButton_LuaFiles->setChecked(!val);
    ui->plainTextEdit_luaLog->setEnabled(val);
}

//enables widgets
void MainWindow::setupPointerTab(bool val)
{
    ui->comboBox_selectRangeToDump->setEnabled(val);
    ui->pushButton_setDumpDir->setEnabled(val);
    ui->lineEdit_dumpDir->setEnabled(val);
    ui->lineEdit_dumpFileName->setEnabled(val);
}

//enables widgets
void MainWindow::setupProcessInfoTab(bool val)
{
    ui->pushButton_refresh_regionList->setEnabled(val);
    ui->tableWidget_memoryRegions->setEnabled(val);
    ui->tableWidget_modules->setEnabled(val);
}

//loads cheat's text into textEdit
void MainWindow::on_listWidget_cheats_itemDoubleClicked(QListWidgetItem *item)
{
    QString fileName = item->text();
    QFile cheat(luaCheatsDir.path() + "/" + fileName);
    if(!cheat.open(QIODevice::ReadOnly)) { QMessageBox::warning(Q_NULLPTR, "Error opening cheat", cheat.errorString()); }
    QTextStream in(&cheat);
    in.setEncoding(QStringConverter::Utf8);
    ui->plainTextEdit_luaCheat->setPlainText(in.readAll());
    cheat.close();
}

void MainWindow::cancel()
{
    on_pushButton_terminateCheats_clicked();
    ui->plainTextEdit_luaLog->appendPlainText(QString::fromStdString(*(Cheats::getInstance().getErrorStr())));
}

void MainWindow::printLuaLog()
{
    ui->plainTextEdit_luaLog->appendPlainText(QString::fromStdString(*(Cheats::getInstance().getLogStr())));
}

void MainWindow::clearLuaLog()
{
    ui->plainTextEdit_luaLog->clear();
}

void MainWindow::setProcessLineEdit(int index)
{
    ui->lineEdit_processName->setText(QString::fromStdWString(this->processList[index+2].get_processName()));

    if(refresh_hook()) //if refresh was successfull...
    {
        //setup_gameInfo();
        //setup_search();
        //setup_pointerSearch();

        ui->pushButton_open_MemViewer->setEnabled(true);
        ui->tableWidget_memoryRegions->setEnabled(true);
        ui->tableWidget_modules->setEnabled(true);
        //ui->pushButton_refresh_regionList->setEnabled(true);

        //setupSearchTab(true);
        setupCheatsTab(true);
        //setupPointerTab(true);
        ui->label_status->setText("PC game connected. Only cheat tab is active.");
    }
}
