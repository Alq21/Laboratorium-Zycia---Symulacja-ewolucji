#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "predator.h"
#include <vector>
#include <memory>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , board(nullptr)
    , world(nullptr)
    , engine(nullptr)
{
    ui->setupUi(this);


    board = new TempBoard(this);

    board->setGeometry(10, 50, 500, 500);


    world = new World(50, 50);
    engine = new SimEngine(world, nullptr); // StatManager narazie nullptr


    std::unique_ptr<Organism> testBug = std::make_unique<Predator>(
        Position{5, 5},
        Color{255, 0, 0},
        100.0, // startEnergy
        200.0, // maxEnergy
        1,     // size
        1,     // speed
        1,     // maxAP
        1,     // generation
        5      // visionRange
        );
    world->addOrganism(std::move(testBug));

    std::vector<Entity*> entitiesToDraw;
    for(const auto& org : world->getOrganisms()) {
        entitiesToDraw.push_back(org.get());
    }
    board->setEntities(entitiesToDraw);

    resize(600, 600);
}

MainWindow::~MainWindow()
{
    delete ui;

    delete engine;
    delete world;
}

void MainWindow::on_StepButton_clicked()
{}
void MainWindow::on_pushButton_clicked()
{

}