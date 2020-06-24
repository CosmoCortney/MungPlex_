#ifndef PROCESSLIST_H
#define PROCESSLIST_H

#include <QDialog>
#include"processinfo.h"
#include<vector>
#include<QListWidget>

namespace Ui {
class ProcessList;
}

class ProcessList : public QDialog
{
    Q_OBJECT

public:
    explicit ProcessList(QWidget *parent = nullptr);
    void setProcessList(std::vector<ProcessInfo>& list);

private:
    Ui::ProcessList *ui;
    std::vector<ProcessInfo> processes;

signals:
    void doubleClicked(int index);

private slots:
    void on_pushButton_ok_clicked();
    void on_pushButton_cancel_clicked();
};

#endif // PROCESSLIST_H
