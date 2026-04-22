#include "MassScheme.h"

QVector<QVector<int>> MassScheme::createEmptyMatrix()
{
    return QVector<QVector<int>>(6, QVector<int>(32, 0));
}

SchemeResult MassScheme::build(const SchemeInput &data)
{
    SchemeResult result;
    result.mass = createEmptyMatrix();
    result.sectorName = "Не определен";

    if (data.activeSquare < 0)
        return result;

    CellPos pos{0, 0, false};

    // ===============================
    // Сектор "Знак"
    // ===============================
    if (data.isSign)
    {
        result.sectorName = "Сектор Знак";

        if (data.variant == 1)
            pos = mapVariant1Sign(data.activeSquare);
        else
            pos = mapVariant2Sign(data.activeSquare);

        if (pos.valid)
        {
            int globalCol = 28 + pos.col;

            if (pos.row >= 0 && pos.row < 6 &&
                globalCol >= 0 && globalCol < 32)
            {
                result.mass[pos.row][globalCol] = 1;
            }
        }

        return result;
    }

    // ===============================
    // Обычные сектора 1..7
    // ===============================
    if (data.kTetr >= 1 && data.kTetr <= 7)
    {
        int sectorStart = (data.kTetr - 1) * 4;
        result.sectorName = QString("Сектор тетрады %1").arg(data.kTetr);

        if (data.variant == 1)
            pos = mapVariant1Normal(data.activeSquare);
        else
            pos = mapVariant2Normal(data.activeSquare);

        if (pos.valid)
        {
            int globalCol = sectorStart + pos.col;

            if (pos.row >= 0 && pos.row < 6 &&
                globalCol >= 0 && globalCol < 32)
            {
                result.mass[pos.row][globalCol] = 1;
            }
        }
    }

    return result;
}

// =======================================================
// ВАРИАНТ 1, ОБЫЧНАЯ СХЕМА
// =======================================================
CellPos MassScheme::mapVariant1Normal(int squareIndex)
{
    switch (squareIndex)
    {
    case 0:  return {0, 0, true}; // p10
    case 1:  return {0, 1, true}; // p11
    case 2:  return {0, 2, true}; // p12
    case 3:  return {0, 3, true}; // p13

    case 4:  return {1, 0, true}; // s10
    case 5:  return {1, 1, true}; // s11
    case 6:  return {1, 2, true}; // s12
    case 7:  return {1, 3, true}; // s13

    case 8:  return {2, 3, true}; // Kk

    case 9:  return {3, 1, true}; // p21
    case 10: return {3, 2, true}; // p22
    case 11: return {3, 3, true}; // p23

    case 12: return {4, 1, true}; // s1
    case 13: return {4, 2, true}; // s2
    case 14: return {4, 3, true}; // s3

    default: return {0, 0, false};
    }
}

// =======================================================
// ВАРИАНТ 2, ОБЫЧНАЯ СХЕМА
// По твоей таблице:
// row0: p61 p62
// row1: a11 a12 a13
// row2: p10 p11 p12 p13
// row3: s10 s11 s12 s13
// row4: p21 p22
// row5: s1 s2 s3
// =======================================================
CellPos MassScheme::mapVariant2Normal(int squareIndex)
{
    switch (squareIndex)
    {
    case 0:  return {0, 1, true}; // p61
    case 1:  return {0, 2, true}; // p62

    case 2:  return {1, 1, true}; // a11
    case 3:  return {1, 2, true}; // a12
    case 4:  return {1, 3, true}; // a13

    case 5:  return {2, 0, true}; // p10
    case 6:  return {2, 1, true}; // p11
    case 7:  return {2, 2, true}; // p12
    case 8:  return {2, 3, true}; // p13

    case 9:  return {3, 0, true}; // s10
    case 10: return {3, 1, true}; // s11
    case 11: return {3, 2, true}; // s12
    case 12: return {3, 3, true}; // s13

    case 13: return {4, 1, true}; // p21
    case 14: return {4, 2, true}; // p22

    case 15: return {5, 1, true}; // s1
    case 16: return {5, 2, true}; // s2
    case 17: return {5, 3, true}; // s3

    default: return {0, 0, false};
    }
}

// =======================================================
// ВАРИАНТ 1, СХЕМА "ЗНАК"
// По твоей схеме:
// row0: p10 p11 p12 [31 пусто]
// row1: s10 s11 s12 s13
// =======================================================
CellPos MassScheme::mapVariant1Sign(int squareIndex)
{
    switch (squareIndex)
    {
    case 0: return {0, 0, true}; // p10
    case 1: return {0, 1, true}; // p11
    case 2: return {0, 2, true}; // p12

    case 3: return {1, 0, true}; // s10
    case 4: return {1, 1, true}; // s11
    case 5: return {1, 2, true}; // s12
    case 6: return {1, 3, true}; // s13

    default: return {0, 0, false};
    }
}

// =======================================================
// ВАРИАНТ 2, СХЕМА "ЗНАК"
// По твоему второму скрину схема знака такая же,
// значит таблица такая же.
// =======================================================
CellPos MassScheme::mapVariant2Sign(int squareIndex)
{
    switch (squareIndex)
    {
    case 0: return {2, 0, true}; // p10
    case 1: return {2, 1, true}; // p11
    case 2: return {2, 2, true}; // p12

    case 3: return {3, 0, true}; // s10
    case 4: return {3, 1, true}; // s11
    case 5: return {3, 2, true}; // s12
    case 6: return {3, 3, true}; // s13

    default: return {0, 0, false};
    }
}
