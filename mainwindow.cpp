#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QTimer>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Inicjalizacja aplikacji symulacyjnej kolegi
    simApp = new SimulationApp(this);

    // POŁĄCZENIE: Kiedy symulacja zrobi krok, Twoja plansza dostaje dane
    connect(simApp, &SimulationApp::tickCompleted, this, [this](long tick) {
        auto entities = simApp->collectEntities();

        if (ui->boardWidget) {
            ui->boardWidget->setEntities(entities);
            // Wymuszamy przerysowanie, gdyby update() wewnątrz setEntities nie wystarczył
            ui->boardWidget->repaint();
        }
    });

    // Obsługa błędów
    connect(simApp, &SimulationApp::errorOccurred, this, [](const QString& msg) {
        qDebug() << "Błąd symulacji:" << msg;
    });

    // Ładowanie konfiguracji
    if (simApp->loadFromFile("example_config.json")) {
        qDebug() << "Konfiguracja załadowana pomyślnie.";

        // WYMUSZENIE PIERWSZEGO WIDOKU:
        // Czekamy 100ms, aż UI się ułoży i pobieramy startowe encje
        QTimer::singleShot(100, this, [this](){
            auto entities = simApp->collectEntities();
            ui->boardWidget->setEntities(entities);
        });
    }

    resize(800, 800);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_startButton_clicked()
{
    if (simApp) simApp->resume();
}

void MainWindow::on_stopButton_clicked()
{
    if (simApp) simApp->pause();
}

void MainWindow::on_pushButton_clicked()
{
    // Ręczny krok (jeśli simApp na to pozwala)
    if (simApp) {
        simApp->resume();
        QTimer::singleShot(50, simApp, &SimulationApp::pause);
    }
}