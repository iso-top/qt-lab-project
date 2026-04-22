#ifndef MASSSCHEME_H
#define MASSSCHEME_H

#include <QVector>
#include <QString>

struct SchemeInput
{
    int variant;        // 1 или 2
    int kTetr;          // 1..7
    bool isSign;        // выбрано ли "Знак"
    int activeSquare;   // выбранный квадрат
};

struct CellPos
{
    int row;
    int col;
    bool valid;
};

struct SchemeResult
{
    QVector<QVector<int>> mass; // 6x32
    QString sectorName;
};

class MassScheme
{
public:
    static SchemeResult build(const SchemeInput &data);

private:
    static QVector<QVector<int>> createEmptyMatrix();

    static CellPos mapVariant1Normal(int squareIndex);
    static CellPos mapVariant2Normal(int squareIndex);

    static CellPos mapVariant1Sign(int squareIndex);
    static CellPos mapVariant2Sign(int squareIndex);
};

#endif // MASSSCHEME_H
