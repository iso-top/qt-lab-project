
#ifndef DATACOLLECTOR_H
#define DATACOLLECTOR_H

#include <QVector>

class MainWindow; // объявляем класс окна

class DataCollector
{
public:

    // функция формирования массива
    static QVector<int> collect(MainWindow *uiWindow);

};

#endif
