#include "DataCollector.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

QVector<int> DataCollector::collect(MainWindow *window)
{
    QVector<int> data(9);

    Ui::MainWindow *ui = window->getUI();

    // 0 – вариант схемы
    data[0] = ui->VariantBtn->currentIndex() + 1;

    // 1 – количество тетрад
    data[1] = ui->comboTetrads->currentText().toInt();

    // 2 – режим работы
    if (ui->btnFullCycle->isChecked())
        data[2] = 0;
    else
        data[2] = 1;

    // 3 – число A
    data[3] = ui->btnA->text().toInt();

    // 4 – число B
    data[4] = ui->btnB->text().toInt();

    // 5 – входной перенос
    data[5] = ui->btnCarry->currentText().toInt();

    // 6 – номер тетрады ошибки
    QString errorText = ui->comboError->currentText();

    if (errorText == "Нет")
        data[6] = -1;
    else if (errorText == "Знак")
        data[6] = ui->comboTetrads->currentText().toInt();
    else
        data[6] = errorText.toInt();

    // 7 – вид ошибки
    if (ui->btnFail->isChecked())
        data[7] = 0;
    else
        data[7] = 1;

    // 8 – константа
    if (ui->btnConst0->isChecked())
        data[8] = 0;
    else
        data[8] = 1;

    return data;
}
