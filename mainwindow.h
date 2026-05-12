#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "tempboard.h"
#include "simengine.h"
#include "world.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_StepButton_clicked();
    void on_pushButton_clicked();

private:
    Ui::MainWindow *ui;
    TempBoard *board;
    World *world;
    SimEngine *engine;
};

#endif // MAINWINDOW_H