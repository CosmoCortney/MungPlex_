#include "processlist.h"
#include "ui_processlist.h"

ProcessList::ProcessList(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ProcessList)
{
    ui->setupUi(this);
}


void ProcessList::setProcessList(std::vector<ProcessInfo> &list)

{
    this->processes = list;

    for(int i = 2; i < processes.size(); ++i)
    {
        ui->listWidget->addItem(QString::fromStdWString(processes[i].get_processName()));
    }
}


/*ProcessList::~ProcessList()
{
    delete ui;
}*/

void ProcessList::on_pushButton_ok_clicked()
{
    int index = ui->listWidget->currentRow();
    emit doubleClicked(index);
    this->close();
}

void ProcessList::on_pushButton_cancel_clicked()
{
    this->close();
}
