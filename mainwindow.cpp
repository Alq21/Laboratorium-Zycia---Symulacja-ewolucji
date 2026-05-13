#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "producer.h"
#include "thermophile.h"
#include "cryophile.h"
#include "environmentparameters.h"
#include "tile.h"
#include <vector>
#include <memory>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , world(nullptr)
    , engine(nullptr)
    , timer(new QTimer(this))
{
    ui->setupUi(this);


    world = new World(25, 25);


    EnvironmentParameters globalParams(20.0, 50.0, 50.0);
    world->setGlobalParameters(globalParams);

    for (int x = 0; x < 25; ++x) {
        for (int y = 0; y < 25; ++y) {
            Tile* tile = world->getTile(Position{x, y});
            if (tile != nullptr) {
                if (x < 8 && y < 8) {

                    tile->getLocalModifiers().modifyParameter("temperature", -30.0);
                } else if (x > 16 && y > 16) {
                    tile->getLocalModifiers().modifyParameter("temperature", 30.0);
                }
            }
        }
    }


    world->populate(10, 2);


    world->addOrganism(std::make_unique<Thermophile>(
        Position{12, 12}, Color{200, 50, 50}, 100.0, 150.0, 12, 2, 5, 1));

    world->addOrganism(std::make_unique<Cryophile>(
        Position{13, 13}, Color{50, 50, 200}, 100.0, 150.0, 12, 2, 5, 1));


    engine = new SimEngine(world, nullptr);
    engine->resume();


    connect(timer, &QTimer::timeout, this, &MainWindow::performStep);


    std::vector<Entity*> entitiesToDraw;
    for(const auto& org : world->getOrganisms()) {
        entitiesToDraw.push_back(org.get());
    }


    ui->boardWidget->setEntities(entitiesToDraw);

    resize(600, 600);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete engine;
    delete world;
}


void MainWindow::performStep()
{
    if (engine) {
        engine->step();

        std::vector<Entity*> entitiesToDraw;
        for(const auto& org : engine->getWorld()->getOrganisms()) {
            entitiesToDraw.push_back(org.get());
        }

        ui->boardWidget->setEntities(entitiesToDraw);
    }
}


void MainWindow::on_pushButton_clicked()
{
    performStep();
}


void MainWindow::on_startButton_clicked()
{
    timer->start(250);
}

void MainWindow::on_stopButton_clicked()
{
    timer->stop();
}