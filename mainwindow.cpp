#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "simulationapp.h"
#include <QDebug>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    board = new TempBoard(this);
    board->setGeometry(10, 50, 500, 500);
    simApp = new SimulationApp(this);
    connect(simApp, &SimulationApp::tickCompleted,
            this, [this](long tick) {
                std::vector<Entity*> entities = simApp->collectEntities();
                qDebug() << "Tick:" << tick << "Entities:" << entities.size();
                board->setEntities(entities);
            });
    connect(simApp, &SimulationApp::errorOccurred,
            this, [](const QString& msg) { qDebug() << "SimError:" << msg; });
    if (simApp->loadFromFile(QString(SOURCE_DIR) + "/example_config.json"))
        simApp->start();

    resize(600, 600);
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::on_StepButton_clicked() {
    simApp->pause();
}

void MainWindow::on_pushButton_clicked() {
    if (simApp->isPaused())
        simApp->resume();
    else
        simApp->pause();
}