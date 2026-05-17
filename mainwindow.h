#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QTimer>
// potem trzeba ten zegar usunąć

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
    void on_resumeButton_clicked();
    void on_startButton_clicked();
    void on_stopButton_clicked();

    void on_stepButton_clicked();
    void on_pauseButton_clicked();


private:
    Ui::MainWindow *ui;
    SimulationApp *simApp;
};

#endif // MAINWINDOW_H