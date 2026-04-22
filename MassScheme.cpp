#include "MassScheme.h"

// Создание пустого массива 5x31, заполненного нулями
QVector<QVector<int>> MassScheme::createEmptyMatrix()
{
    return QVector<QVector<int>>(5, QVector<int>(32, 0));
}

// Главная функция построения массива
SchemeResult MassScheme::build(const SchemeInput &data)
{
    SchemeResult result;
    result.mass = createEmptyMatrix();
    result.sectorName = "Не определен";

    // Если квадрат не выбран, возвращаем нулевой массив
    if (data.activeSquare < 0)
        return result;

    CellPos pos{0, 0, false};

    // -----------------------------------------
    // Если выбрана схема "Знак"
    // Последние столбцы массива: 28, 29, 30
    // -----------------------------------------
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

            if (pos.row >= 0 && pos.row < 5 &&
                globalCol >= 0 && globalCol < 32)
            {
                result.mass[pos.row][globalCol] = 1;
            }
        }

        return result;
    }

    // -----------------------------------------
    // Обычные сектора
    // Количество значащих тетрад определяет сектор:
    // 0 -> столбцы 0..3
    // 1 -> столбцы 4..7
    // 2 -> столбцы 8..11
    // ...
    // 6 -> столбцы 24..27
    // -----------------------------------------
    int sectorStart = data.kTetr * 4;
    result.sectorName = QString("Сектор тетрады %1").arg(data.kTetr);

    if (data.variant == 1)
        pos = mapVariant1Normal(data.activeSquare);
    else
        pos = mapVariant2Normal(data.activeSquare);

    if (pos.valid)
    {
        int globalCol = sectorStart + pos.col;

        if (pos.row >= 0 && pos.row < 5 &&
            globalCol >= 0 && globalCol < 32)
        {
            result.mass[pos.row][globalCol] = 1;
        }
    }

    return result;
}

// =======================================================
// ВАРИАНТ 1, ОБЫЧНАЯ СХЕМА
// Здесь квадрат переводится в строку и столбец внутри сектора
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
// Пока сделана как временная заглушка.
// Потом сюда нужно будет вписать точное соответствие
// по твоей второй схеме.
// =======================================================
CellPos MassScheme::mapVariant2Normal(int squareIndex)
{
    switch (squareIndex)
    {
    case 0:  return {0, 0, true};
    case 1:  return {0, 1, true};
    case 2:  return {0, 2, true};
    case 3:  return {0, 3, true};

    case 4:  return {1, 0, true};
    case 5:  return {1, 1, true};
    case 6:  return {1, 2, true};
    case 7:  return {1, 3, true};

    case 8:  return {2, 0, true};
    case 9:  return {2, 1, true};
    case 10: return {2, 2, true};
    case 11: return {2, 3, true};

    case 12: return {3, 0, true};
    case 13: return {3, 1, true};
    case 14: return {3, 2, true};
    case 15: return {3, 3, true};

    case 16: return {4, 0, true};
    case 17: return {4, 1, true};

    default: return {0, 0, false};
    }
}

// =======================================================
// ВАРИАНТ 1, СХЕМА "ЗНАК"
// Пока сделана как временная логика для 8 квадратов.
// Под реальную схему можно будет поменять позже.
// col здесь только 0..2, потому что сектор "Знак" = 3 столбца
// =======================================================
CellPos MassScheme::mapVariant1Sign(int squareIndex)
{
    switch (squareIndex)
    {
    case 0: return {0, 0, true};
    case 1: return {0, 1, true};
    case 2: return {0, 2, true};

    case 3: return {1, 0, true};
    case 4: return {1, 1, true};
    case 5: return {1, 2, true};

    case 6: return {2, 0, true};
    case 7: return {2, 1, true};

    default: return {0, 0, false};
    }
}

// =======================================================
// ВАРИАНТ 2, СХЕМА "ЗНАК"
// Тоже временная заглушка
// =======================================================
CellPos MassScheme::mapVariant2Sign(int squareIndex)
{
    switch (squareIndex)
    {
    case 0: return {0, 0, true};
    case 1: return {0, 1, true};
    case 2: return {0, 2, true};

    case 3: return {1, 0, true};
    case 4: return {1, 1, true};
    case 5: return {1, 2, true};

    case 6: return {2, 0, true};
    case 7: return {2, 1, true};

    default: return {0, 0, false};
    }
}
