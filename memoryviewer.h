#ifndef MEMORYVIEWER_H
#define MEMORYVIEWER_H

#include <QDialog>
#include"hook.h"
#include<QDebug>
#include<windows.h>
#include<QString>
#include"rangelayout.h"
#include<vector>
#include"dataconversion.h"
#include<QTimer>
#include "ui_memoryviewer.h"

namespace Ui{
class MemoryViewer;
}

class MemoryViewer : public QDialog
{
    Q_OBJECT

public:
    explicit MemoryViewer(QWidget *parent = nullptr);
    ~MemoryViewer();
    void setHook(Hook& hook){ this->hook = hook; }
    void set_rangeLayouts(std::vector<RangeLayout>& rangeLayouts){ this->rangeLayouts = rangeLayouts; }
    void readMemory(char* addr);
    void setBE(bool val){ this->isBE = val; ui->checkBox_BE->setChecked(val); }
    void set_selectedRange(int val){ this->selectedRange = val; }
    void set_rangeComboBox();
    void setOffset(unsigned long long baseOffset, unsigned long long virtualBase);
    template<typename dType> void poke(char* address, dType val)
    {
        WriteProcessMemory(hook.getHandle(), reinterpret_cast<LPVOID>(address), &val, sizeof(val), 0);
    }
    void pokeRoutine();

private slots:
    void auto_refresh();
    void auto_poke();
    void on_checkBox_autoRefresh_clicked(bool checked);
    void on_checkBox_BE_clicked(bool checked);
    void on_comboBox_rangeSelect_currentIndexChanged(int index);
    void on_lineEdit_goTo_returnPressed();
    void on_checkBox_autoPoke_clicked(bool checked);
    void on_pushButton_poke_clicked();
    void on_pushButton_pageUp_clicked();
    void on_pushButton_pageDown_clicked();
    void on_tableWidget_memory_currentCellChanged(int currentRow, int currentColumn, int previousRow, int previousColumn);
    void on_comboBox_pokeType_currentIndexChanged(int index);

private:
    Hook hook;
    std::vector<RangeLayout> rangeLayouts;
    unsigned long long selectedRange = 0;
    bool isBE = false;
    QTimer* timeRead;
    QTimer* timePoke;
    Ui::MemoryViewer *ui;
    unsigned long long currentOffsetFromBase = 0;
    unsigned long long virtualBaseAddress = 0;
    int selectedType = 0;
    unsigned char* tempMemory;
};

#endif // MEMORYVIEWER_H
