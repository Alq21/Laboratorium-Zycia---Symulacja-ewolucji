#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFile>
#include <iostream>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    simApp = new SimulationApp(this);

    // Załaduj config
    QString configPath = "example_config.json";
    if (!QFile::exists(configPath)) {
        configPath = QString(SOURCE_DIR) + "/example_config.json";
    }

    if (simApp->loadFromFile(configPath)) {
        std::cout << "Config loaded successfully!" << std::endl;
        auto entities = simApp->collectEntities();
        ui->boardWidget->setEntities(entities);
    } else {
        std::cout << "ERROR: " << simApp->lastError().toStdString() << std::endl;
    }


    ui->boardWidget->setSimApp(simApp);


    connect(simApp, &SimulationApp::tickCompleted, this, [this](long tick){
        auto entities = simApp->collectEntities();
        ui->boardWidget->setEntities(entities);
    });

    resize(900, 780);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_startButton_clicked()
{
    if (simApp) {
        std::cout << "START clicked" << std::endl;
        simApp->startSimulation();
    }
}

void MainWindow::on_stopButton_clicked()
{
    if (simApp) {
        std::cout << "STOP clicked" << std::endl;
        simApp->stopSimulation();
    }
}

void MainWindow::on_pauseButton_clicked()
{
    if (simApp) {
        std::cout << "PAUSE clicked" << std::endl;
        simApp->pauseSimulation();
    }
}

void MainWindow::on_resumeButton_clicked()
{
    if (simApp) {
        std::cout << "RESUME clicked" << std::endl;
        simApp->resumeSimulation();
    }
}

void MainWindow::on_stepButton_clicked()
{
    if (simApp) {
        std::cout << "STEP clicked" << std::endl;
        simApp->stepSimulation();

        // Odśwież widok
        auto entities = simApp->collectEntities();
        ui->boardWidget->setEntities(entities);
    }
}