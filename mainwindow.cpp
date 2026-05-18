#include "mainwindow.h"
#include "simengine.h"
#include "ui_mainwindow.h"
#include "statpanel.h"
#include "climateevents.h"  // DODANE: dla ClimateEventType
#include <QFile>
#include <iostream>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    simApp = new SimulationApp(this);

    // Wczytanie konfiguracji
    configPath = QStringLiteral("example_config.json");
    if (!QFile::exists(configPath))
        configPath = QString(SOURCE_DIR) + QStringLiteral("/example_config.json");

    if (simApp->loadFromFile(configPath)) {
        std::cout << "Konfiguracja zaladowana pomyslnie!" << std::endl;
        refreshBoard();
    } else {
        std::cout << "BLAD: " << simApp->lastError().toStdString() << std::endl;
    }

    // Podpięcie widgetu planszy
    ui->boardWidget->setSimApp(simApp);
    ui->boardWidget->setShowEnvironmentParameters(
        ui->showEnvironmentCheckBox->isChecked());

    connect(ui->showEnvironmentCheckBox, &QCheckBox::toggled, this,
            [this](bool checked) {
                ui->boardWidget->setShowEnvironmentParameters(checked);
            });

    // Sygnały SimulationApp → GUI
    connect(simApp, &SimulationApp::tickCompleted, this, [this](long) {
        refreshBoard();
    });

    connect(simApp, &SimulationApp::simulationStarted,
            this, &MainWindow::updateControlButtons);
    connect(simApp, &SimulationApp::simulationStopped,
            this, &MainWindow::updateControlButtons);
    connect(simApp, &SimulationApp::simulationPaused,
            this, &MainWindow::updateControlButtons);
    connect(simApp, &SimulationApp::simulationResumed,
            this, &MainWindow::updateControlButtons);

    // Zmiana zakładki — odświeżamy statystyki gdy przełączamy na "Statystyki"
    connect(ui->tabWidget, &QTabWidget::currentChanged,
            this, &MainWindow::onTabChanged);

    updateControlButtons();
    resize(960, 820);
}

MainWindow::~MainWindow()
{
    delete ui;
}

// ---------------------------------------------------------------------------
void MainWindow::refreshBoard()
{
    ui->boardWidget->setSnapshot(simApp->collectSnapshot());
}

void MainWindow::refreshStats()
{
    ui->statsPanel->refresh(simApp->collectStats());
}

void MainWindow::updateControlButtons()
{
    const bool running = simApp && simApp->isRunning();
    const bool paused  = simApp && simApp->isPaused();

    ui->startButton->setText(running
        ? QStringLiteral("Restart")
        : QStringLiteral("Start"));
    ui->pauseButton->setText(paused
        ? QStringLiteral("Wznów")
        : QStringLiteral("Pauza"));
    ui->pauseButton->setEnabled(running);

    // Gdy symulacja zapauzowana i jestesmy na zakladce Statystyki — odswiez
    if (paused && ui->tabWidget->currentIndex() == 1)
        refreshStats();
}

// ---------------------------------------------------------------------------
void MainWindow::onTabChanged(int index)
{
    // index 1 == zakładka "Statystyki"
    if (index == 1) {
        refreshStats();
    }
}

// ---------------------------------------------------------------------------
void MainWindow::on_startButton_clicked()
{
    if (!simApp) return;

    if (simApp->isRunning()) {
        std::cout << "RESTART klikniety" << std::endl;
        simApp->restartSimulation(configPath);
        refreshBoard();
    } else {
        std::cout << "START klikniety" << std::endl;
        simApp->startSimulation();
    }
    updateControlButtons();
}

void MainWindow::on_pauseButton_clicked()
{
    if (!simApp || !simApp->isRunning()) return;

    if (simApp->isPaused()) {
        std::cout << "WZNOW klikniety" << std::endl;
        simApp->resumeSimulation();
    } else {
        std::cout << "PAUZA klikniety" << std::endl;
        simApp->pauseSimulation();
    }
    updateControlButtons();
}

void MainWindow::on_stepButton_clicked()
{
    if (!simApp) return;
    std::cout << "KROK klikniety" << std::endl;
    simApp->stepSimulation();
    refreshBoard();

    // Odśwież statystyki po kroku, jeśli zakładka Statystyki jest aktywna
    if (ui->tabWidget->currentIndex() == 1)
        refreshStats();
}

// ========== DODANE: Implementacje slotów dla parametrów i wydarzeń ==========

void MainWindow::on_tempSlider_valueChanged(int value) {
    ui->tempLabel->setText(QString("Temperatura: ").arg(value));
}

void MainWindow::on_humiditySlider_valueChanged(int value) {
    ui->humidityLabel->setText(QString("Wilgotność:").arg(value));
}

void MainWindow::on_sunlightSlider_valueChanged(int value) {
    ui->sunlightLabel->setText(QString("Nasłonecznienie:").arg(value));
}

// void MainWindow::on_autoEventsCheckBox_toggled(bool checked) {
//     if (simApp && simApp->getEngine()) {
//         simApp->getEngine()->getClimateEvents()->setAutoEnabled(checked);
//         std::cout << "Auto wydarzenia: " << (checked ? "ON" : "OFF") << std::endl;
//     }
// }

void MainWindow::on_heatwaveButton_clicked() {
    if (simApp && simApp->getEngine()) {
        simApp->getEngine()->getClimateEvents()->trigger(ClimateEventType::Heatwave);
        std::cout << "Wywołano FALĘ UPAŁÓW!" << std::endl;
        refreshBoard();
    }
}

void MainWindow::on_freezeButton_clicked() {
    if (simApp && simApp->getEngine()) {
        simApp->getEngine()->getClimateEvents()->trigger(ClimateEventType::Freeze);
        std::cout << "Wywołano MRÓZ!" << std::endl;
        refreshBoard();
    }
}

void MainWindow::on_droughtButton_clicked() {
    if (simApp && simApp->getEngine()) {
        simApp->getEngine()->getClimateEvents()->trigger(ClimateEventType::Drought);
        std::cout << "Wywołano SUSZĘ!" << std::endl;
        refreshBoard();
    }
}

void MainWindow::on_stormButton_clicked() {
    if (simApp && simApp->getEngine()) {
        simApp->getEngine()->getClimateEvents()->trigger(ClimateEventType::Storm);
        std::cout << "Wywołano BURZĘ!" << std::endl;
        refreshBoard();
    }
}
