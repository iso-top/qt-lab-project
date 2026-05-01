// mainwindow.cpp
/*
=========================================================
Файл mainwindow.cpp

Реализует класс MainWindow, который отвечает за:

• работу интерфейса программы
• переключение схем
• обработку действий пользователя
• работу с квадратами на схемах
• передачу данных в MassScheme

Это основной файл логики GUI.
=========================================================
*/
#include "mainwindow.h"
#include "teacher/ChetnostVar1.h"
#include "ui_mainwindow.h"
#include "DataCollector.h" // сбор параметров интерфейса
#include "MassScheme.h" // сбор параметров интерфейса

#include <QIntValidator> // ограничение ввода чисел встроенная библиотека qt
#include <cmath> // математические функции встроенная библиотека qt
#include <QFont> // работа со шрифтами
#include <QPixmap>  // работа с изображениями
#include <QPushButton> // кнопки интерфейса
#include <QMessageBox> // всплывающие окна
#include <QPoint> // координаты (x,y)
#include <QDebug> // вывод в консоль
#include <QFile>
#include <QTextStream>//для работы с текстовым файлом
#include <QSignalBlocker>
#include <QCoreApplication>
#include <QFile>
#include <QTextStream>
class NoLeadingZeroIntValidator : public QIntValidator
{
public:
    NoLeadingZeroIntValidator(int bottom, int top, QObject *parent = nullptr)
        : QIntValidator(bottom, top, parent)
    {
    }

    State validate(QString &input, int &pos) const override
    {
        Q_UNUSED(pos);

        // Пустое поле разрешаем как промежуточное состояние
        if (input.isEmpty())
            return Intermediate;

        // Разрешаем пользователю начать ввод отрицательного числа
        if (input == "-")
            return Intermediate;

        // Плюс в начале запрещаем
        if (input.startsWith("+"))
            return Invalid;

        bool negative = input.startsWith("-");
        QString digits = negative ? input.mid(1) : input;

        if (digits.isEmpty())
            return Intermediate;

        // Разрешаем только цифры
        for (QChar ch : digits) {
            if (!ch.isDigit())
                return Invalid;
        }

        // Запрещаем ведущие нули:
        // 00, 0005, 002000, -01, -0003
        if (digits.length() > 1 && digits.startsWith("0"))
            return Invalid;

        // Запрещаем -0
        if (negative && digits == "0")
            return Invalid;

        bool ok = false;
        int value = input.toInt(&ok);

        if (!ok)
            return Invalid;

        // Проверяем диапазон по количеству тетрад
        if (value < bottom() || value > top())
            return Invalid;

        return Acceptable;
    }
};
/*
=========================================================
Конструктор главного окна

Здесь происходит вся начальная настройка:

• загрузка интерфейса
• установка ограничений ввода
• формирование списка ошибок
• настройка кнопок
• инициализация квадратов
• загрузка схемы
=========================================================
*/
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    /*
        Группа кнопок выбора типа ошибки
    */
    errorGroup = new QButtonGroup(this);
    errorGroup->setExclusive(true);
    errorGroup->addButton(ui->btnFail);
    errorGroup->addButton(ui->btnReject);

    /*
        Группа кнопок выбора констант
    */
    constGroup = new QButtonGroup(this);
    constGroup->setExclusive(true);
    constGroup->addButton(ui->btnConst0);
    constGroup->addButton(ui->btnConst1);

    /*
        При изменении типа ошибки обновляем доступность констант
    */
    connect(ui->btnFail, &QPushButton::toggled, this, [this]() {
        updateConstButtonsState();
        lockBottomScheme();
    });

    connect(ui->btnReject, &QPushButton::toggled, this, [this]() {
        updateConstButtonsState();
        lockBottomScheme();
    });

    connect(ui->btnConst0, &QPushButton::toggled, this, [this]() {
        lockBottomScheme();
    });

    connect(ui->btnConst1, &QPushButton::toggled, this, [this]() {
        lockBottomScheme();
    });

    connect(ui->btnA, &QLineEdit::textChanged, this, [this]() {
        lockBottomScheme();
    });

    connect(ui->btnB, &QLineEdit::textChanged, this, [this]() {
        lockBottomScheme();
    });

    connect(ui->btnCarry, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this]() {
        lockBottomScheme();
    });

    applyButtonStyles();

    initSquares();
    initSquarePositions();

    updateNumberLimits();
    updateErrorCombo();

    ui->btnFullCycle->setChecked(false);
    ui->btnPair->setChecked(false);

    ui->btnA->setEnabled(false);
    ui->btnB->setEnabled(false);
    ui->btnCarry->setEnabled(false);

    updateScheme(ui->VariantBtn->currentIndex());
    updateErrorControlsState();
}

/*
=========================================================
Координаты квадратов для всех схем

Каждый квадрат — это кнопка поверх изображения.

Для каждой схемы задаются свои координаты.
=========================================================
*/
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

/*
=========================================================
Скрытие лишних квадратов

Разные схемы имеют разное количество квадратов всего квадратов 18.
=========================================================
*/
void MainWindow::hideUnusedSquares(int usedCount)
{
    for (int i = 0; i < squares.size(); i++) {

        if (i < usedCount)
            squares[i]->show();
        else
            squares[i]->hide();
    }
}
/*
=========================================================
Установка координат квадратов
=========================================================
*/
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

/*
=========================================================
Кнопка "Принять"(вверхняя) - собирает данные из интерфейса и формирует массив параметров

Проверяет параметры и собирает данные
=========================================================
*/
void MainWindow::on_ApplyBtn_clicked()
{
    if (isFullCycleTooLong()) {
        showFullCycleWarning();
        lockBottomScheme();
        return;
    }

    if (!validateTopControls()) {
        lockBottomScheme();
        return;
    }

    QVector<int> data = DataCollector::collect(this);

    qDebug() << "Массив параметров:" << data;

    unlockBottomScheme();
}
/*
 =========================================================
  Эта функция отвечает за формирование списка
  в поле "Ошибка в тетраде №"
 =========================================================
*/
void MainWindow::updateErrorCombo()
{
    QSignalBlocker blocker(ui->comboError);

    int count = ui->comboTetrads->currentText().toInt();

    ui->comboError->clear();
    ui->comboError->addItem("Нет");

    for (int i = 0; i < count; i++)
    {
        ui->comboError->addItem(QString::number(i));
    }

    ui->comboError->addItem("Знак");

    ui->comboError->setCurrentIndex(0);

    updateErrorControlsState();
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
    updateNumberLimits();

    if (isFullCycleTooLong()) {
        showFullCycleWarning();

        ui->btnFullCycle->setChecked(false);
    }

    lockBottomScheme();
}

/*
=========================================================
Обновление схемы в зависимости от варианта
=========================================================
*/
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

    // Для схемы "Знак" скоректирован размер
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

    lockBottomScheme();
}
/*
=========================================================
Стили кнопок общий вид кнопки
=========================================================
*/

void MainWindow::applyButtonStyles()
{
    QString modeStyle =
        "QPushButton { background: lightgray; border:1px solid black; }"
        "QPushButton:checked { background: rgb(120,255,120); }";

    QString errorStyle =
        "QPushButton { background: lightgray; border:1px solid black; }"
        "QPushButton:checked { background: rgb(120,255,120); }"
        "QPushButton:disabled { background: rgb(180,180,180); border:1px solid gray; }";

    QString constStyle =
        "QPushButton { background: lightgray; border:1px solid black; }"
        "QPushButton:checked { background: rgb(120,255,120); }"
        "QPushButton:disabled { background: rgb(180,180,180); border:1px solid gray; }";

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
    QString helpPath = QCoreApplication::applicationDirPath() + "/help.txt";

    QFile file(helpPath);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(
            this,
            "Ошибка",
            "Не удалось открыть файл подсказки:\n" + helpPath
        );
        return;
    }

    QTextStream in(&file);
    in.setEncoding(QStringConverter::Utf8);

    QString helpText = in.readAll();

    QMessageBox msg(this);
    msg.setWindowTitle("О программе");
    msg.setTextFormat(Qt::PlainText);
    msg.setText(helpText);

    QFont font;
    font.setPointSize(14);
    msg.setFont(font);

    msg.setMinimumWidth(500);
    msg.exec();
}
// константы не вкл пока не выбрано чтото другое помимо нет в "Ошибка в тетраде №"
void MainWindow::updateConstButtonsState()
{
    QString errorText = ui->comboError->currentText();

    bool hasError = !errorText.isEmpty() && errorText != "Нет";
    bool enabled = hasError && ui->btnReject->isChecked();

    ui->btnConst0->setEnabled(enabled);
    ui->btnConst1->setEnabled(enabled);

    if (!enabled) {
        if (constGroup) {
            constGroup->setExclusive(false);
        }

        ui->btnConst0->setChecked(false);
        ui->btnConst1->setChecked(false);

        if (constGroup) {
            constGroup->setExclusive(true);
        }
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
    if (ui->btnFullCycle->isChecked()) {

        if (isFullCycleTooLong()) {
            showFullCycleWarning();

            ui->btnFullCycle->setChecked(false);
            return;
        }

        clearRightMode();
    }
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

    lockBottomScheme();
}


void MainWindow::updateNumberLimits()
{
    int tetrads = ui->comboTetrads->currentText().toInt();

    int maxValue = static_cast<int>(pow(10, tetrads)) - 1;

    ui->btnA->setValidator(
        new NoLeadingZeroIntValidator(-maxValue, maxValue, ui->btnA)
    );

    ui->btnB->setValidator(
        new NoLeadingZeroIntValidator(-maxValue, maxValue, ui->btnB)
    );
}


bool MainWindow::isFullCycleTooLong() const
{
    int tetrads = ui->comboTetrads->currentText().toInt();

    return ui->btnFullCycle->isChecked() && tetrads > 4;
}

void MainWindow::showFullCycleWarning()
{
    QMessageBox::warning(
        this,
        "Предупреждение",
        "Работа в режиме \"Полный цикл\" при количестве значащих тетрад больше 4 "
        "будет занимать десятки минут.\n\n"
        "Выберите режим \"Пара чисел\"."
    );
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

//проверка верхних параметров
bool MainWindow::validateTopControls()
{
    bool fullCycle = ui->btnFullCycle->isChecked();
    bool pairMode = ui->btnPair->isChecked();

    if (!fullCycle && !pairMode) {
        QMessageBox::warning(
            this,
            "Ошибка",
            "Выберите режим работы:\n"
            "\"Полный цикл\" или \"Пара чисел\"."
        );
        return false;
    }

    if (pairMode) {
        if (ui->btnA->text().trimmed().isEmpty() ||
            ui->btnB->text().trimmed().isEmpty())
        {
            QMessageBox::warning(
                this,
                "Ошибка",
                "Введите значения A и B."
            );
            return false;
        }

        if (!ui->btnA->hasAcceptableInput() ||
            !ui->btnB->hasAcceptableInput())
        {
            QMessageBox::warning(
                this,
                "Ошибка",
                "Значения A и B выходят за допустимые пределы."
            );
            return false;
        }
    }

    QString errorText = ui->comboError->currentText();

    if (errorText != "Нет") {
        if (!ui->btnFail->isChecked() && !ui->btnReject->isChecked()) {
            QMessageBox::warning(
                this,
                "Ошибка",
                "Выберите вид ошибки:\n"
                "\"Сбой\" или \"Отказ\"."
            );
            return false;
        }

        if (ui->btnReject->isChecked()) {
            if (!ui->btnConst0->isChecked() && !ui->btnConst1->isChecked()) {
                QMessageBox::warning(
                    this,
                    "Ошибка",
                    "Для отказа выберите константу:\n"
                    "\"Константа 0\" или \"Константа 1\"."
                );
                return false;
            }
        }
    }

    return true;
}

/*
=========================================================
Выбор квадрата
=========================================================
*/
void MainWindow::selectSquare(int index)
{
    if (!topApplyAccepted)
        return;

    if (index < 0 || index >= squares.size())
        return;

    if (!squares[index]->isEnabled())
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
//функция блокировки нижней схемы
void MainWindow::setSchemeSquaresEnabled(bool enabled)
{
    for (auto square : squares) {
        square->setEnabled(enabled);
    }
}
void MainWindow::resetActiveSquare()
{
    if (activeSquare >= 0 && activeSquare < squares.size()) {
        setSquareColor(squares[activeSquare], "rgb(120,255,120)");
    }

    activeSquare = -1;
}
void MainWindow::lockBottomScheme()
{
    topApplyAccepted = false;

    resetActiveSquare();

    setSchemeSquaresEnabled(false);

    ui->ApplyBtn2->setEnabled(false);
}
void MainWindow::unlockBottomScheme()
{
    topApplyAccepted = true;

    resetActiveSquare();

    /*
        Если выбрано "Нет", то ошибки нет,
        значит выбирать квадрат на схеме не нужно.
    */
    bool hasError = (ui->comboError->currentText() != "Нет");

    setSchemeSquaresEnabled(hasError);

    ui->ApplyBtn2->setEnabled(true);
}

void MainWindow::on_comboError_currentIndexChanged(int index)
{
    Q_UNUSED(index);

    updateErrorControlsState();
    updateScheme(ui->VariantBtn->currentIndex());

    lockBottomScheme();
}
//функция сброса кнопок
void MainWindow::clearErrorButtons()
{
    if (errorGroup) {
        errorGroup->setExclusive(false);
    }

    ui->btnFail->setChecked(false);
    ui->btnReject->setChecked(false);

    if (errorGroup) {
        errorGroup->setExclusive(true);
    }

    if (constGroup) {
        constGroup->setExclusive(false);
    }

    ui->btnConst0->setChecked(false);
    ui->btnConst1->setChecked(false);

    if (constGroup) {
        constGroup->setExclusive(true);
    }
}

//функция вкл отключения контейнера вид ошибки
void MainWindow::updateErrorControlsState()
{
    QString errorText = ui->comboError->currentText();

    bool hasError = !errorText.isEmpty() && errorText != "Нет";

    ui->VariantError->setEnabled(hasError);

    if (!hasError) {
        clearErrorButtons();
        return;
    }

    updateConstButtonsState();
}
/*
=============================================================
Формирование массива схемы при нажатии нижней кнопки принять
=============================================================
*/
void MainWindow::on_ApplyBtn2_clicked()
{
    if (!topApplyAccepted) {
        QMessageBox::warning(
            this,
            "Ошибка",
            "Сначала выберите параметры сверху и нажмите верхнюю кнопку \"Принять\"."
        );
        return;
    }

    if (ui->comboError->currentText() != "Нет" && activeSquare < 0) {
        QMessageBox::warning(
            this,
            "Ошибка",
            "Выберите элемент на схеме."
        );
        return;
    }

    SchemeInput data;

    data.variant = ui->VariantBtn->currentIndex() + 1;
    data.kTetr = ui->comboTetrads->currentText().toInt();
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

    runTeacherCalculation();
}
void MainWindow::runTeacherCalculation()
{
    QVector<int> data = DataCollector::collect(this);

    if (data.size() < 9) {
        QMessageBox::warning(
            this,
            "Ошибка",
            "DataCollector вернул массив меньше 9 элементов."
        );
        return;
    }

    int Mass1[9];

    for (int i = 0; i < 9; i++) {
        Mass1[i] = data[i];
    }

    SchemeInput input;

    input.variant = ui->VariantBtn->currentIndex() + 1;
    input.kTetr = ui->comboTetrads->currentText().toInt();
    input.isSign = (ui->comboError->currentText() == "Знак");
    input.activeSquare = activeSquare;

    SchemeResult schemeResult = MassScheme::build(input);

    int Mass2[6][32];

    for (int row = 0; row < 6; row++) {
        for (int col = 0; col < 32; col++) {
            Mass2[row][col] = schemeResult.mass[row][col];
        }
    }

    int resultCode = runChetnostVar1(Mass1, Mass2);

    if (resultCode == 0) {
        QMessageBox::information(
            this,
            "Готово",
            "Расчёт завершён.\n"
            "Результаты записаны в файл statistica.txt."
        );
    } else {
        QMessageBox::warning(
            this,
            "Ошибка",
            "Расчёт завершился с ошибкой.\n"
            "Код ошибки: " + QString::number(resultCode)
        );
    }
}
//объвление квадратиков 
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
