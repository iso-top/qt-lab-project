#ifndef MASSSCHEME_H
#define MASSSCHEME_H

#include <QVector>
#include <QString>

// Входные данные для построения массива
struct SchemeInput
{
    int variant;        // 1 или 2
    int kTetr;          // количество значащих тетрад: 0..6
    bool isSign;        // true, если выбрано "Знак"
    int activeSquare;   // номер выбранного красного квадрата
};

// Позиция элемента внутри одного сектора
struct CellPos
{
    int row;    // строка
    int col;    // столбец внутри сектора
    bool valid; // найдено ли соответствие
};

// Результат работы
struct SchemeResult
{
    QVector<QVector<int>> mass; // массив 5x31
    QString sectorName;         // название сектора для вывода в консоль
};

class MassScheme
{
public:
    static SchemeResult build(const SchemeInput &data);

private:
    static QVector<QVector<int>> createEmptyMatrix();

    // Таблицы соответствия для обычных схем
    static CellPos mapVariant1Normal(int squareIndex);
    static CellPos mapVariant2Normal(int squareIndex);

    // Таблицы соответствия для схемы "Знак"
    static CellPos mapVariant1Sign(int squareIndex);
    static CellPos mapVariant2Sign(int squareIndex);
};

#endif // MASSSCHEME_H
