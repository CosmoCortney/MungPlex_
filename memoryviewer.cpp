#include "memoryviewer.h"

MemoryViewer::MemoryViewer(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MemoryViewer)
{
    ui->setupUi(this);

    ui->tableWidget_memory->setColumnCount(16);
    ui->tableWidget_memory->setHorizontalHeaderLabels(QStringList() << "00"  << "01"  << "02"  << "03"  << "04"  << "05"  << "06"  << "07"
                                                                    << "08"  << "09"  << "0A"  << "0B"  << "0C"  << "0D"  << "0E"  << "0F" );
    ui->tableWidget_memory->setRowCount(16);
    ui->tableWidget_memory->setVerticalHeaderLabels(QStringList() << "00" << "10" << "20" << "30" << "40" << "50" << "60" << "70"
                                                                  << "80" << "90" << "A0" << "B0" << "C0" << "D0" << "E0" << "F0");


    ui->comboBox_pokeType->addItem("Int_8", DataConversion::int8);
    ui->comboBox_pokeType->addItem("Int_16", DataConversion::int16);
    ui->comboBox_pokeType->addItem("Int_32", DataConversion::int32);
    ui->comboBox_pokeType->addItem("Int_64", DataConversion::int64);
    ui->comboBox_pokeType->addItem("Float Single", DataConversion::float_);
    ui->comboBox_pokeType->addItem("Float Double", DataConversion::double_);

    ui->tableWidget_memory->horizontalHeader()->resizeSections(QHeaderView::ResizeMode::ResizeToContents);

    timeRead = new QTimer(this);
    timePoke = new QTimer(this);
    connect(timeRead, SIGNAL(timeout()), this, SLOT(auto_refresh()));
    connect(timePoke, SIGNAL(timeout()), this, SLOT(auto_poke()));
}

MemoryViewer::~MemoryViewer()
{
    delete timeRead;
    delete timePoke;
    delete ui;
}

void MemoryViewer::readMemory(char* addr)
{
    virtualBaseAddress = rangeLayouts[selectedRange].get_virtualAddress();
    unsigned char tempVal;
    unsigned char* tempAddr;
    QStringList rowHeaders;
    tempMemory = reinterpret_cast<unsigned char*>(malloc(0x100));
    ReadProcessMemory(hook.getHandle(), reinterpret_cast<LPCVOID>(addr), tempMemory, 0x100, nullptr);

    for(int col = 0; col < 16; ++col)
    {
        for(int row = 0; row < 16; ++row)
        {
            tempVal = *((tempMemory+(col)+(row*16)));
            ui->tableWidget_memory->setItem(row, col, new QTableWidgetItem(QString::number(((tempVal)), 16).rightJustified(2, '0')));
            tempAddr = reinterpret_cast<unsigned char*>(currentOffsetFromBase & 0xFFFFFFFFFFFFFFF0);
            tempAddr += virtualBaseAddress + (row*16);
            rowHeaders << QString::number(reinterpret_cast<long long>(tempAddr), 16);
            //qDebug() << (unsigned int*)(tempMemory+(col*4)+(row*16));
        }
    }
    ui->tableWidget_memory->setVerticalHeaderLabels(rowHeaders);
}

void MemoryViewer::on_checkBox_autoRefresh_clicked(bool checked)
{
    checked ? timeRead->start(16) : timeRead->stop();
}

void MemoryViewer::auto_refresh()
{
    readMemory((char*)(rangeLayouts[selectedRange].get_address() + currentOffsetFromBase));
}

void MemoryViewer::on_checkBox_BE_clicked(bool checked)
{
    isBE = (checked ? true : false);
}

void MemoryViewer::set_rangeComboBox()
{
    for(int i = 0; i < rangeLayouts.size(); i++)
    {
        ui->comboBox_rangeSelect->addItem(QString::number(rangeLayouts[i].get_virtualAddress(), 16));
        ui->comboBox_rangeSelect->setItemData(i, rangeLayouts[i].get_address());
    }
}

void MemoryViewer::on_comboBox_rangeSelect_currentIndexChanged(int index)
{
    readMemory((char*)(rangeLayouts[index].get_address() + currentOffsetFromBase));
    selectedRange = ui->comboBox_rangeSelect->currentIndex();
}

void MemoryViewer::on_lineEdit_goTo_returnPressed()
{
    unsigned long long tempOffset = ui->lineEdit_goTo->text().toULongLong(Q_NULLPTR, 16) - ui->comboBox_rangeSelect->currentText().toULongLong(Q_NULLPTR, 16);
    setOffset(tempOffset, ui->comboBox_rangeSelect->currentText().toULongLong(Q_NULLPTR, 16));
    readMemory((char*)(rangeLayouts[selectedRange].get_address() + (currentOffsetFromBase & 0xFFFFFFFFFFFFFFF0)));
}

void MemoryViewer::setOffset(unsigned long long baseOffset, unsigned long long virtualBase)
{
    this->currentOffsetFromBase = baseOffset;
    this->virtualBaseAddress = virtualBase;
}

void MemoryViewer::pokeRoutine()
{
    unsigned long long mem = (rangeLayouts[selectedRange].get_address() + (ui->lineEdit_pokeAddress->text().toULongLong(Q_NULLPTR, 16) - virtualBaseAddress));
    char* addr = (char*)mem;

    switch(selectedType)
    {
        case DataConversion::int8: poke<char>(addr, (char)ui->lineEdit_pokeValue->text().toUShort(Q_NULLPTR, 16)); break;
        case DataConversion::int16: {
            unsigned short val = ui->lineEdit_pokeValue->text().toUShort(Q_NULLPTR, 16);
            poke<short>(addr, isBE ? DataConversion::swapBytes(val) : val);
        } break;
        case DataConversion::int32: {
            unsigned int val = ui->lineEdit_pokeValue->text().toUInt(Q_NULLPTR, 16);
            poke<int>(addr, isBE ? DataConversion::swapBytes(val) : val);
        } break;
        case DataConversion::int64: {
            unsigned long long val = ui->lineEdit_pokeValue->text().toULongLong(Q_NULLPTR, 16);
            poke<long long>(addr, isBE ? DataConversion::swapBytes(val) : val);
        } break;
        case DataConversion::float_: {
            float val = ui->lineEdit_pokeValue->text().toFloat(Q_NULLPTR);
            poke<float>(addr, isBE ? DataConversion::swapBytes(val) : val);
        } break;
        case DataConversion::double_: {
            double val = ui->lineEdit_pokeValue->text().toDouble(Q_NULLPTR);
            poke<double>(addr, isBE ? DataConversion::swapBytes(val) : val);
        } break;
    }
    auto_refresh();
}

void MemoryViewer::on_checkBox_autoPoke_clicked(bool checked)
{
    checked ? timePoke->start(16) : timePoke->stop();
}

void MemoryViewer::auto_poke() { pokeRoutine(); }

void MemoryViewer::on_pushButton_poke_clicked()
{
    pokeRoutine();
}

void MemoryViewer::on_pushButton_pageUp_clicked()
{
    currentOffsetFromBase -= 0x10;
    readMemory(reinterpret_cast<char*>(rangeLayouts[selectedRange].get_address()+currentOffsetFromBase));
}

void MemoryViewer::on_pushButton_pageDown_clicked()
{
    currentOffsetFromBase += 0x10;
    readMemory(reinterpret_cast<char*>(rangeLayouts[selectedRange].get_address()+currentOffsetFromBase));
}

void MemoryViewer::on_tableWidget_memory_currentCellChanged(int currentRow, int currentColumn, int previousRow, int previousColumn)
{
    bool BE = ui->checkBox_BE->isChecked();

    unsigned long long vAddr = virtualBaseAddress + currentOffsetFromBase + currentColumn + currentRow*0x10;
    QString val = QString::number(vAddr, 16);
    ui->lineEdit_pokeAddress->setText(val);

    unsigned long long addr = (unsigned long long)tempMemory + currentColumn + currentRow*0x10;;

    val.clear();

    switch(selectedType)
    {
        case DataConversion::int8: val = QString::number(*(unsigned char*)(addr), 16).rightJustified(2, '0'); break;
        case DataConversion::int16:{
            unsigned short temp = *reinterpret_cast<unsigned short*>(addr);
            val = QString::number(BE ? DataConversion::swapBytes<unsigned short>(temp) : temp, 16).rightJustified(4, '0');
        }break;
        case DataConversion::int32:{
            unsigned int temp = *reinterpret_cast<unsigned int*>(addr);
            val = QString::number(BE ? DataConversion::swapBytes<unsigned int>(temp) : temp, 16).rightJustified(8, '0');
        }break;
        case DataConversion::int64:{
            unsigned long long temp = *reinterpret_cast<unsigned long long*>(addr);
            val = QString::number(BE ? DataConversion::swapBytes<unsigned long long>(temp) : temp, 16).rightJustified(16, '0');
        }break;
        case DataConversion::float_:{
            float temp = *reinterpret_cast<float*>(addr);
            val = QString::number(BE ? DataConversion::swapBytes<float>(temp) : temp);
        }break;
        case DataConversion::double_:{
            double temp = *reinterpret_cast<double*>(addr);
            val = QString::number(BE ? DataConversion::swapBytes<double>(temp) : temp);
        }break;
    }

    ui->lineEdit_pokeValue->setText(val);
}

void MemoryViewer::on_comboBox_pokeType_currentIndexChanged(int index)
{
    selectedType = ui->comboBox_pokeType->itemData(index).value<int>();
}
