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
    void on_pushButton_clicked();   // Ręczny krok symulacji (Step)
    void on_startButton_clicked();  // Rozpoczęcie symulacji (Start)
    void on_stopButton_clicked();   // Zatrzymanie symulacji (Stop)


private:
    Ui::MainWindow *ui;
    SimulationApp *simApp;
};

#endif // MAINWINDOW_H