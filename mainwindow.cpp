// mainwindow.cpp
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "DataCollector.h"
#include "MassScheme.h"

#include <QIntValidator>
#include <cmath>
#include <QFont>
#include <QPixmap>
#include <QPushButton>
#include <QMessageBox>
#include <QPoint>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{

    ui->setupUi(this);
    updateNumberLimits();
    updateErrorCombo();

    applyButtonStyles();
    initSquares();
    initSquarePositions();

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
void MainWindow::initSquarePositions()
{
    // Координаты квадратов для Scheme_1 (x,y)
    scheme1Points = {
        QPoint(502, 453),  // sq1 ТРУ
        QPoint(689, 453),  // sq2 ТРУ
        QPoint(875, 453),  // sq3 ТРУ 2 1
        QPoint(1064, 453),  // sq4 ТРУ
        QPoint(502, 495),  // sq5 ТРУ
        QPoint(689, 495),  // sq6 ТРУ
        QPoint(875, 495),  // sq7 ТРУ
        QPoint(1064, 495),  // sq8 ТРУ
        QPoint(1005, 770),  // sq9 ТРУ
        QPoint(835, 827),  // sq10 ТРУ
        QPoint(1023, 807),  // sq11 ТРУ
        QPoint(1190, 825),  // sq12 ТРУ
        QPoint(835, 869),  // sq13 ТРУ
        QPoint(1023, 869),  // sq14 ТРУ
        QPoint(1190, 869)   // sq15 ТРУ
    };

    // Координаты квадратов для Scheme_2 (x,y)
    scheme2Points = {
        QPoint(590, 453),  // sq1 ТРУ
        QPoint(780, 435),  // sq2 ТРУ
        QPoint(610, 514),  // sq3 ТРУ
        QPoint(803, 514),  // sq4 ТРУ
        QPoint(995, 510),  // sq5 ТРУ
        QPoint(520, 604),  // sq6 ТРУ
        QPoint(715, 604),  // sq7 ТРУ
        QPoint(910, 604),  // sq8 ТРУ
        QPoint(1105, 604),  // sq9 ТРУ
        QPoint(520, 650),  // sq10 ТРУ
        QPoint(715, 650),  // sq11 ТРУ
        QPoint(910, 650),  // sq12 ТРУ
        QPoint(1105, 650),  // sq13 ТРУ
        QPoint(867, 800),  // sq14 ТРУ
        QPoint(1037, 800),   // sq15 ТРУ
        QPoint(890, 858),   // sq16 ТРУ
        QPoint(1060, 875),   // sq17 ТРУ
        QPoint(1252, 875)   // sq18 ТРУ
    };

    // Координаты для квадратов Scheme_Znak (x,y)
    schemeZnakPoints = {
        QPoint(467, 610),  // sq1 ТРУ
        QPoint(791, 610),  // sq2 ТРУ
        QPoint(1118, 610),  // sq3 ТРУ
        QPoint(500, 730),  // sq4 ТРУ
        QPoint(824, 730),  // sq5 ТРУ
        QPoint(1151, 730),  // sq6 ТРУ
        QPoint(1484, 730),  // sq7 ТРУ
    };
}
//функция скрытия квадратов

void MainWindow::hideUnusedSquares(int usedCount)
{
    for (int i = 0; i < squares.size(); i++) {

        if (i < usedCount)
            squares[i]->show();
        else
            squares[i]->hide();
    }
}

void MainWindow::applySquarePositions(const QVector<QPoint> &points)
{
    int count = qMin(squares.size(), points.size());

    for (int i = 0; i < count; i++) {
        squares[i]->move(points[i]);
    }
}
MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_ApplyBtn_clicked()
{
    // Проверяем, включён ли режим "Полный цикл"
    bool fullCycleEnabled = ui->btnFullCycle->isChecked();

    // Получаем текст из поля "Ошибка в тетраде №"
    QString errorText = ui->comboError->currentText();

    // Если выбрано "Нет" или "Знак", то это не число
    bool ok = false;
    int errorNumber = errorText.toInt(&ok);

    // Если режим "Полный цикл" включён
    // и номер ошибки 4 или больше -> показать предупреждение
    if (fullCycleEnabled && ok && errorNumber >= 4)
    {
        QMessageBox::warning(
            this,
            "Предупреждение",
            "Работа в режиме \"Полный цикл\" будет занимать десятки минут.\n"
            "Просьба перейти в режим \"Пара чисел\"."
        );
        return;
    }

    // Здесь дальше твоя обычная логика кнопки "Принять"
    QVector<int> data = DataCollector::collect(this);

    // пример вывода в консоль
    qDebug() << "Массив параметров:" << data;
}
/*
 =========================================================
  Эта функция отвечает за формирование списка
  в поле "Ошибка в тетраде №"
 =========================================================
*/
void MainWindow::updateErrorCombo()
{
    int count = ui->comboTetrads->currentText().toInt();

    ui->comboError->clear();
    ui->comboError->addItem("Нет");

    for (int i = 0; i < count; i++)
    {
        ui->comboError->addItem(QString::number(i));
    }

    ui->comboError->addItem("Знак");

    // после обновления списка ошибок сразу обновляем схему
    updateScheme(ui->VariantBtn->currentIndex());
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
    updateNumberLimits(); // новая функция ограничения
}

//Переключение схемы в зависимости от варианта
void MainWindow::updateScheme(int index)
{
    QString path;
    bool isZnak = false;

    // Выбираем картинку схемы
    if (ui->comboError->currentText() == "Знак") {
        path = ":/image/Scheme_Znak.png";
        isZnak = true;
    } else {
        switch (index) {
        case 0:
            path = ":/image/Scheme_1.png";
            break;
        case 1:
            path = ":/image/Scheme_2.png";
            break;
        default:
            path = ":/image/Scheme_1.png";
            break;
        }
    }

    QPixmap pix(path);

    if (pix.isNull()) {
        ui->schemeLabel->setText("Ошибка загрузки");
        return;
    }

    ui->schemeLabel->setText("");
    ui->schemeLabel->setAlignment(Qt::AlignCenter);
    ui->schemeLabel->setScaledContents(false);

    // Масштаб картинки внутри label
    double scale = 0.95;

    int width  = int(ui->schemeLabel->width() * scale);
    int height = int(ui->schemeLabel->height() * scale);

    // Для схемы "Знак" можно сделать размер чуть больше
    if (isZnak) {
        width  = int(ui->schemeLabel->width() * 0.98);
        height = int(ui->schemeLabel->height() * 0.98);
    }

    ui->schemeLabel->setPixmap(
        pix.scaled(
            width,
            height,
            Qt::KeepAspectRatio,
            Qt::SmoothTransformation
        )
    );

    // После смены схемы двигаем квадраты и скрываем лишние
    if (isZnak) {
        applySquarePositions(schemeZnakPoints);
        hideUnusedSquares(7);
    } else {
        switch (index) {
        case 0:
            applySquarePositions(scheme1Points);
            hideUnusedSquares(15);
            break;

        case 1:
            applySquarePositions(scheme2Points);
            hideUnusedSquares(18);
            break;

        default:
            applySquarePositions(scheme1Points);
            hideUnusedSquares(15);
            break;
        }
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
void MainWindow::updateNumberLimits()
{
    // сколько тетрад выбрано
    int tetrads = ui->comboTetrads->currentText().toInt();

    // считаем максимум
    int maxValue = pow(10, tetrads) - 1;

    // создаём валидатор
    QIntValidator *validator = new QIntValidator(-maxValue, maxValue, this);

    // применяем к полям A и B
    ui->btnA->setValidator(validator);
    ui->btnB->setValidator(validator);
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
        ui->sq15,
        ui->sq16,
        ui->sq17,
        ui->sq18
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

void MainWindow::on_comboError_currentIndexChanged(int index)
{
    Q_UNUSED(index);
    updateScheme(ui->VariantBtn->currentIndex());
}

void MainWindow::on_ApplyBtn2_clicked()
{
    SchemeInput data;

    data.variant = ui->VariantBtn->currentIndex() + 1;
    data.kTetr = ui->comboTetrads->currentText().toInt();   // 1..7
    data.isSign = (ui->comboError->currentText() == "Знак");
    data.activeSquare = activeSquare;

    SchemeResult result = MassScheme::build(data);

    qDebug() << "======================";
    qDebug() << "variant =" << data.variant;
    qDebug() << "kTetr =" << data.kTetr;
    qDebug() << "isSign =" << data.isSign;
    qDebug() << "activeSquare =" << data.activeSquare;
    qDebug() << "sector =" << result.sectorName;

    for (int i = 0; i < result.mass.size(); i++) {
        qDebug() << result.mass[i];
    }

    qDebug() << "======================";
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
void MainWindow::on_sq16_clicked() { selectSquare(15); }
void MainWindow::on_sq17_clicked() { selectSquare(16); }
void MainWindow::on_sq18_clicked() { selectSquare(17); }
