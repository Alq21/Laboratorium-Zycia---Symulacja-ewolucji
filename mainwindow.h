#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include "world.h"
#include "simengine.h"
// potem trzeba ten zegar usunąć
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButton_clicked();   // Ręczny krok symulacji (Step)
    void on_startButton_clicked();  // Rozpoczęcie symulacji (Start)
    void on_stopButton_clicked();   // Zatrzymanie symulacji (Stop)
    void performStep();             // Funkcja wykonująca i rysująca jeden cykl

private:
    Ui::MainWindow *ui;
    World *world;
    SimEngine *engine;
    QTimer *timer;
};

#endif // MAINWINDOW_H