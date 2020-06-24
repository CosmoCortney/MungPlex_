#include "conversionwindow.h"
#include "ui_conversionwindow.h"

ConversionWindow::ConversionWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConversionWindow)
{
    ui->setupUi(this);
}

ConversionWindow::~ConversionWindow()
{
    delete ui;
}

void ConversionWindow::on_pushButton_toFloatHex_clicked()
{
    if(ui->radioButton_floatHex->isChecked())
    {
        float input = ui->lineEdit_floatFloat->text().toFloat(Q_NULLPTR);
        unsigned int result = DataConversion::floatingPointToBinaryInt<float, unsigned int>(input);
        ui->lineEdit_floatHex->setText(QString::number(result, 16).toUpper());
    }
    else
    {
        double input = ui->lineEdit_floatFloat->text().toDouble(Q_NULLPTR);
        unsigned long long result = DataConversion::floatingPointToBinaryInt<double, unsigned long long>(input);
        ui->lineEdit_floatHex->setText(QString::number(result, 16).toUpper());
    }
}

void ConversionWindow::on_pushButton_hexToFloat_clicked()
{
    if(ui->radioButton_floatHex->isChecked())
    {
        unsigned int input = ui->lineEdit_floatHex->text().toUInt(Q_NULLPTR, 16);
        double result = static_cast<double>(DataConversion::binaryIntToFloatingPoint<float, unsigned int>(input));
        ui->lineEdit_floatFloat->setText(QString::number(result));
    }
    else
    {
        unsigned long long input = ui->lineEdit_floatHex->text().toULongLong(Q_NULLPTR, 16);
        double result = DataConversion::binaryIntToFloatingPoint<double, unsigned long long>(input);
        ui->lineEdit_floatFloat->setText(QString::number(result));
    }
}

void ConversionWindow::on_pushButton_clicked()
{
    if(ui->radioButton_int16->isChecked())
    {
        unsigned short input = ui->lineEdit_LE_BE_in->text().toUShort(Q_NULLPTR, 16);
        unsigned short result = DataConversion::swapBytes(input);
        ui->lineEdit_LE_BE_out->setText(QString::number(result, 16));
    }
    else if(ui->radioButton_int32->isChecked())
    {
        unsigned int input = ui->lineEdit_LE_BE_in->text().toUInt(Q_NULLPTR, 16);
        unsigned int result = DataConversion::swapBytes(input);
        ui->lineEdit_LE_BE_out->setText(QString::number(result, 16));
    }
    else
    {
        unsigned long long input = ui->lineEdit_LE_BE_in->text().toULongLong(Q_NULLPTR, 16);
        unsigned long long result = DataConversion::swapBytes(input);
        ui->lineEdit_LE_BE_out->setText(QString::number(result, 16));
    }
}
