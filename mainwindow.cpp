// mainwindow.cpp
#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QIntValidator>
#include <cmath>
#include <QFont>
#include <QPixmap>
#include <QPushButton>
#include <QMessageBox>
#include <QPoint>

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
    // Координаты для Scheme_1
    scheme1Points = {
        QPoint(530, 471),  // sq1
        QPoint(1000, 595),  // sq2
        QPoint(1000, 640),  // sq3
        QPoint(1000, 675),  // sq4
        QPoint(1055, 630),  // sq5
        QPoint(1050, 745),  // sq6
        QPoint(1085, 605),  // sq7
        QPoint(1150, 680),  // sq8
        QPoint(1180, 725),  // sq9
        QPoint(1180, 770),  // sq10
        QPoint(1250, 640),  // sq11
        QPoint(1300, 600),  // sq12
        QPoint(1300, 690),  // sq13
        QPoint(1290, 780),  // sq14
        QPoint(1045, 815)   // sq15
    };

    // Координаты для Scheme_2
    scheme2Points = {
        QPoint(1010, 545),
        QPoint(990, 580),
        QPoint(990, 625),
        QPoint(990, 660),
        QPoint(1045, 615),
        QPoint(1040, 735),
        QPoint(1075, 590),
        QPoint(1140, 665),
        QPoint(1170, 710),
        QPoint(1170, 755),
        QPoint(1240, 625),
        QPoint(1290, 585),
        QPoint(1290, 675),
        QPoint(1280, 765),
        QPoint(1035, 805)
    };

    // Координаты для Scheme_Znak
    schemeZnakPoints = {
        QPoint(1030, 575),
        QPoint(1030, 615),
        QPoint(1030, 655),
        QPoint(1030, 695),
        QPoint(1065, 635),
        QPoint(1065, 775),
        QPoint(1100, 625),
        QPoint(1160, 685),
        QPoint(1190, 730),
        QPoint(1190, 775),
        QPoint(1250, 645),
        QPoint(1310, 610),
        QPoint(1310, 695),
        QPoint(1300, 785),
        QPoint(1060, 825)
    };
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

    // После смены схемы двигаем квадраты
    if (ui->comboError->currentText() == "Знак") {
        applySquarePositions(schemeZnakPoints);
    } else {
        switch (index) {
        case 0:
            applySquarePositions(scheme1Points);
            break;
        case 1:
            applySquarePositions(scheme2Points);
            break;
        default:
            applySquarePositions(scheme1Points);
            break;
        }
    }
    // коэффициент размера относительно label
    double scale = 0.95;

    int width  = int(ui->schemeLabel->width() * scale);
    int height = int(ui->schemeLabel->height() * scale);

    // для схемы "Знак" можно сделать немного больше, но все равно меньше label
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

void MainWindow::on_comboError_currentIndexChanged(int index)
{
    Q_UNUSED(index);
    updateScheme(ui->VariantBtn->currentIndex());
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
