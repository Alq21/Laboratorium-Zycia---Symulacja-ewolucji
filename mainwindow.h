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
    
    // DODANE: Slidery parametrów środowiska
    void on_tempSlider_valueChanged(int value);
    void on_humiditySlider_valueChanged(int value);
    void on_sunlightSlider_valueChanged(int value);
    
    // DODANE: Wydarzenia klimatyczne

    void on_heatwaveButton_clicked();
    void on_freezeButton_clicked();
    void on_droughtButton_clicked();
    void on_stormButton_clicked();

private:
    void updateControlButtons();
    void refreshBoard();
    void refreshStats();            // przekazuje dane do StatPanel

    Ui::MainWindow* ui;
    SimulationApp*  simApp;
    QString         configPath;
};

#endif // MAINWINDOW_H
