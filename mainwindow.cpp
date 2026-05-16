#include "mainwindow.h"
#include "organism.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QTimer>
#include <QFile>
#include <iostream>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);


    simApp = new SimulationApp(this);

    QTimer *updateTimer = new QTimer(this);
    connect(updateTimer, &QTimer::timeout, this, [this](){
        if (simApp && simApp->isRunning()) {
            auto entities = simApp->collectEntities();

            std::cout << "\n[MainWindow] Updating board with " << entities.size() << " entities" << std::endl;

            // Policz organizmy
            int orgCount = 0;
            for (auto* e : entities) {
                if (Organism* org = dynamic_cast<Organism*>(e)) {
                    if (org->getIsAlive()) {
                        orgCount++;
                        std::cout << "  Organism at (" << org->getPosition().x << "," << org->getPosition().y << ")" << std::endl;
                    }
                }
            }
            std::cout << "  Total organisms to draw: " << orgCount << std::endl;

            ui->boardWidget->setEntities(entities);
        }
    });
    updateTimer->start(300);
    //

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
    if (!simApp->isRunning()) {
        simApp->start();
    } else if (simApp->isPaused()) {
        simApp->resume();
    }
}

void MainWindow::on_stopButton_clicked()
{
    if (simApp) simApp->stop();
}

void MainWindow::on_pushButton_clicked()
{
    if (simApp) {
        simApp->onTick();}

      // (const auto& org : engine->getWorld()->getOrganisms()) {
      //       entitiesToDraw.push_back(org.get());
      //   }        std::vector<Entity*> entitiesToDraw;
      //   for

      //   ui->boardWidget->setEntities(entitiesToDraw);
}
