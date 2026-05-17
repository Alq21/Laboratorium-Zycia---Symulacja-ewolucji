#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "simulationapp.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;

private slots:
    void on_startButton_clicked();
    void on_pauseButton_clicked();
    void on_stepButton_clicked();
    void onTabChanged(int index);   // odswiezanie statystyk po zmianie zakladki

private:
    void updateControlButtons();
    void refreshBoard();
    void refreshStats();            // przekazuje dane do StatPanel

    Ui::MainWindow* ui;
    SimulationApp*  simApp;
    QString         configPath;
};

#endif // MAINWINDOW_H
