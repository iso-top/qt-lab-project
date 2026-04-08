// mainwindow.cpp
#include "mainwindow.h"
#include "ui_mainwindow.h"


#include <QFont>
#include <QPixmap>
#include <QPushButton>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{

    ui->setupUi(this);

    updateErrorCombo();

    applyButtonStyles();
    initSquares();

    updateScheme(ui->VariantBtn->currentIndex());

    ui->btnFullCycle->setChecked(false);
    ui->btnPair->setChecked(false);

    ui->btnA->setEnabled(false);
    ui->btnB->setEnabled(false);
    ui->btnCarry->setEnabled(false);

    errorGroup = new QButtonGroup(this);
    errorGroup->setExclusive(true);
    errorGroup->addButton(ui->btnFail);
    errorGroup->addButton(ui->btnReject);

    constGroup = new QButtonGroup(this);
    constGroup->setExclusive(true);
    constGroup->addButton(ui->btnConst0);
    constGroup->addButton(ui->btnConst1);

    connect(ui->btnFail, &QPushButton::toggled, this, [this]() {
        updateConstButtonsState();
    });

    connect(ui->btnReject, &QPushButton::toggled, this, [this]() {
        updateConstButtonsState();
    });

    updateConstButtonsState();
}

MainWindow::~MainWindow()
{
    delete ui;
}

/*
 =========================================================
  Эта функция отвечает за формирование списка
  в поле "Ошибка в тетраде №"
 =========================================================
*/
void MainWindow::updateErrorCombo()
{
    /*
      Получаем количество тетрад из комбобокса
      "Количество значащих тетрад"
    */
    int count = ui->comboTetrads->currentText().toInt();

    /*
      Очищаем старые значения
    */
    ui->comboError->clear();

    /*
      Первый пункт всегда "Нет"
    */
    ui->comboError->addItem("Нет");

    /*
      Добавляем номера тетрад
      от 0 до count-1
    */
    for (int i = 0; i < count; i++)
    {
        ui->comboError->addItem(QString::number(i));
    }

    /*
      Последний пункт — "Знак"
    */
    ui->comboError->addItem("Знак");
}


/*
 =========================================================
  Этот слот связан с комбобоксом
  "Количество значащих тетрад"

  Когда пользователь меняет значение,
  мы пересчитываем список ошибок.
 =========================================================
*/
void MainWindow::on_comboTetrads_currentIndexChanged(int index)
{
    Q_UNUSED(index);

    updateErrorCombo();
}
//Переключение схемы в зависимости от варианта
void MainWindow::updateScheme(int index)
{
    QString path;

    switch (index) {
    case 0: path = ":/image/Scheme_1.png"; break;
    case 1: path = ":/image/Scheme_2.png"; break;
    default: path = ":/image/Scheme_1.png"; break;
    }

    QPixmap pix(path);

    if (pix.isNull()) {
        ui->schemeLabel->setText("Ошибка загрузки");
    } else {
        ui->schemeLabel->setText("");
        ui->schemeLabel->setPixmap(pix);
        ui->schemeLabel->setScaledContents(true);
    }
}

//функция что обновляет вариант в зависимости от индекс переменной
void MainWindow::on_VariantBtn_currentIndexChanged(int index)
{
    updateScheme(index);
}


void MainWindow::applyButtonStyles()
{
    QString modeStyle =
        "QPushButton { background: lightgray; border:1px solid black; }"
        "QPushButton:checked { background: rgb(120,255,120); }";

    QString errorStyle =
        "QPushButton { background: lightgray; border:1px solid black; }"
        "QPushButton:checked { background: rgb(120,255,120); }";

    QString constStyle =
        "QPushButton { background: lightgray; border:1px solid black; }"
        "QPushButton:checked { background: rgb(120,255,120); }"
        "QPushButton:disabled { background: rgb(80,80,80); }";

    ui->btnFullCycle->setStyleSheet(modeStyle);
    ui->btnPair->setStyleSheet(modeStyle);

    ui->btnFail->setStyleSheet(errorStyle);
    ui->btnReject->setStyleSheet(errorStyle);

    ui->btnConst0->setStyleSheet(constStyle);
    ui->btnConst1->setStyleSheet(constStyle);
}

//подсказка что вылазит когда нажимаешь на кнопку "?"
void MainWindow::on_HelpBtn_clicked()
{
    QMessageBox msg;

    msg.setWindowTitle("О программе");

    msg.setText(
        "Программа для вычислений\n"
        "- выбор варианта схемы;\n"
        "- выбор режима работы;\n"
        "- выбор состояния элементов;\n"
        "- отображение активного узла на схеме.\n\n"
        "\n"
        "-----------------------"
    );

    QFont font;
    font.setPointSize(14);   // размер текста
    msg.setFont(font);

    msg.setMinimumWidth(500); // ширина окна

    msg.exec();
}
void MainWindow::updateConstButtonsState()
{
    bool enabled = ui->btnReject->isChecked();

    ui->btnConst0->setEnabled(enabled);
    ui->btnConst1->setEnabled(enabled);

    if (!enabled) {
        ui->btnConst0->setChecked(false);
        ui->btnConst1->setChecked(false);
    }
}

void MainWindow::clearRightMode()
{
    ui->btnPair->setChecked(false);
    ui->btnA->setEnabled(false);
    ui->btnB->setEnabled(false);
    ui->btnCarry->setEnabled(false);
}

void MainWindow::enableRightMode()
{
    ui->btnA->setEnabled(true);
    ui->btnB->setEnabled(true);
    ui->btnCarry->setEnabled(true);
}

void MainWindow::clearLeftMode()
{
    ui->btnFullCycle->setChecked(false);
}

void MainWindow::on_btnFullCycle_clicked()
{
    if (ui->btnFullCycle->isChecked())
        clearRightMode();
}

void MainWindow::on_btnPair_clicked()
{
    if (ui->btnPair->isChecked()) {
        clearLeftMode();
        enableRightMode();
    } else {
        ui->btnA->setEnabled(false);
        ui->btnB->setEnabled(false);
        ui->btnCarry->setEnabled(false);
    }
}

void MainWindow::initSquares()
{
    squares = {
        ui->sq1,
        ui->sq2,
        ui->sq3,
        ui->sq4,
        ui->sq5,
        ui->sq6,
        ui->sq7,
        ui->sq8,
        ui->sq9,
        ui->sq10,
        ui->sq11,
        ui->sq12,
        ui->sq13,
        ui->sq14,
        ui->sq15
    };

    for (auto b : squares)
        setSquareColor(b, "rgb(120,255,120)");
}

void MainWindow::setSquareColor(QPushButton *button, const QString &color)
{
    button->setStyleSheet("background:" + color + "; border:1px solid black;");
}

void MainWindow::selectSquare(int index)
{
    if (index < 0 || index >= squares.size())
        return;

    if (activeSquare == index) {
        setSquareColor(squares[index], "rgb(120,255,120)");
        activeSquare = -1;
        return;
    }

    if (activeSquare != -1)
        setSquareColor(squares[activeSquare], "rgb(120,255,120)");

    setSquareColor(squares[index], "red");
    activeSquare = index;
}

//объвление квадратиков для первого варианта
void MainWindow::on_sq1_clicked()  { selectSquare(0); }
void MainWindow::on_sq2_clicked()  { selectSquare(1); }
void MainWindow::on_sq3_clicked()  { selectSquare(2); }
void MainWindow::on_sq4_clicked()  { selectSquare(3); }
void MainWindow::on_sq5_clicked()  { selectSquare(4); }
void MainWindow::on_sq6_clicked()  { selectSquare(5); }
void MainWindow::on_sq7_clicked()  { selectSquare(6); }
void MainWindow::on_sq8_clicked()  { selectSquare(7); }
void MainWindow::on_sq9_clicked()  { selectSquare(8); }
void MainWindow::on_sq10_clicked() { selectSquare(9); }
void MainWindow::on_sq11_clicked() { selectSquare(10); }
void MainWindow::on_sq12_clicked() { selectSquare(11); }
void MainWindow::on_sq13_clicked() { selectSquare(12); }
void MainWindow::on_sq14_clicked() { selectSquare(13); }
void MainWindow::on_sq15_clicked() { selectSquare(14); }
