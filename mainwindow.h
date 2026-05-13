#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "simulationapp.h"
#include "tempboard.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;

private slots:
    void on_StepButton_clicked();
    void on_pushButton_clicked();

private:
    Ui::MainWindow* ui;
    SimulationApp* simApp = nullptr;
    TempBoard* board = nullptr;
};

#endif // MAINWINDOW_H