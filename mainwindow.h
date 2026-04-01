// mainwindow.h
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QVector>
#include <QString>
#include <QButtonGroup>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_btnFullCycle_clicked();
    void on_btnPair_clicked();
    void on_VariantBtn_currentIndexChanged(int index);
    void on_HelpBtn_clicked();
    void on_sq1_clicked();
    void on_sq2_clicked();
    void on_sq3_clicked();
    void on_sq4_clicked();
    void on_sq5_clicked();
    void on_sq6_clicked();
    void on_sq7_clicked();
    void on_sq8_clicked();
    void on_sq9_clicked();
    void on_sq10_clicked();
    void on_sq11_clicked();
    void on_sq12_clicked();
    void on_sq13_clicked();
    void on_sq14_clicked();
    void on_sq15_clicked();

private:
    Ui::MainWindow *ui;

    QVector<QPushButton*> squares;
    int activeSquare = -1;

    QButtonGroup *errorGroup;
    QButtonGroup *constGroup;

    void initSquares();
    void setSquareColor(QPushButton *button, const QString &color);
    void selectSquare(int index);

    void clearRightMode();
    void enableRightMode();
    void clearLeftMode();

    void applyButtonStyles();
    void updateConstButtonsState();
    void updateScheme(int index);
};

#endif // MAINWINDOW_H
