#ifndef CONVERSIONWINDOW_H
#define CONVERSIONWINDOW_H

#include <QDialog>
#include"dataconversion.h"
#include<QString>
#include<QDebug>

namespace Ui {
class ConversionWindow;
}

class ConversionWindow : public QDialog
{
    Q_OBJECT

public:
    explicit ConversionWindow(QWidget *parent = nullptr);
    ~ConversionWindow();

private slots:
    void on_pushButton_toFloatHex_clicked();
    void on_pushButton_hexToFloat_clicked();
    void on_pushButton_clicked();

private:
    Ui::ConversionWindow *ui;
};

#endif // CONVERSIONWINDOW_H
