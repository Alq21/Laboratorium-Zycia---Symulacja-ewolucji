#include "simulationapp.h"
#include "simulationbuilder.h"
#include "simengine.h"
#include "statmanager.h"
#include "world.h"
#include "organism.h"
#include "predator.h"
#include "tile.h"
#include "normaltile.h"
#include "abundanttile.h"
#include "poisontile.h"
#include "impassabletile.h"
#include "omnivore.h"

#include <QMutexLocker>
#include <QThread>
#include <iostream>

SimulationApp::SimulationApp(QObject* parent)
    : QThread(parent)
    , builder(std::make_unique<SimulationBuilder>())
    , running(false)
    , paused(false)
    , interpolation(1.0)
    , tickIntervalMs(500)
{
}

SimulationApp::~SimulationApp()
{
    stopSimulation();
    wait();
}

bool SimulationApp::loadFromFile(const QString& path)
{
    error.clear();

    WorldConfig config = builder->loadConfig(path);
    if (builder->hasError()) {
        error = builder->lastError();
        emit errorOccurred(error);
        return false;
    }

    {
        QMutexLocker locker(&worldMutex);
        world   = builder->buildWorld(config);
        stats   = std::make_unique<StatManager>();
        engine  = std::make_unique<SimEngine>(world.get(), stats.get());
    }

    loadedConfigPath = path;

    std::cout << "\n=== KONFIGURACJA ZALADOWANA ===" << std::endl;
    std::cout << "Organizmy: " << collectSnapshot().organisms.size() << std::endl;

    return true;
}

void SimulationApp::startSimulation()
{
    if (running) return;

    {
        QMutexLocker locker(&worldMutex);
        if (!world || !engine) return;
        engine->resume();
    }

    running = true;
    paused  = false;
    interpolation = 1.0;
    start();
    emit simulationStarted();
}

void SimulationApp::stopSimulation()
{
    if (!running) return;

    running = false;
    paused  = false;
    interpolation = 1.0;
    wait();
    emit simulationStopped();
}

void SimulationApp::restartSimulation(const QString& configPath)
{
    const QString path = configPath.isEmpty() ? loadedConfigPath : configPath;
    if (path.isEmpty()) return;

    const bool wasRunning = running;
    if (wasRunning) {
        running = false;
        paused  = false;
        interpolation = 1.0;
        wait();
    }

    if (!loadFromFile(path)) {
        if (wasRunning) emit simulationStopped();
        return;
    }

    if (!wasRunning) {
        startSimulation();
        return;
    }

    {
        QMutexLocker locker(&worldMutex);
        if (engine) engine->resume();
    }

    running = true;
    paused  = false;
    interpolation = 1.0;
    start();
    emit simulationStarted();
}

void SimulationApp::pauseSimulation()
{
    if (!running || paused) return;
    paused = true;
    emit simulationPaused();
}

void SimulationApp::resumeSimulation()
{
    if (!running || !paused) return;
    paused = false;
    emit simulationResumed();
}

void SimulationApp::stepSimulation()
{
    long tick = 0;
    int  predatorCount = 0, producerCount = 0;
    bool hasPopulation = false;

    {
        QMutexLocker locker(&worldMutex);
        if (!engine || !world) return;

        builder->flushSpawnQueue(world.get());
        engine->step();

        if (stats) {
            stats->takeSnapshot(world.get(), engine->getCurrentTick());
            if (stats->hasData()) {
                const Snapshot& snap = stats->getLatestSnapshot();
                predatorCount = snap.predatorCount;
                producerCount = snap.producerCount;
                hasPopulation = true;
            }
        }

        tick = engine->getCurrentTick();
    }

    interpolation = 1.0;
    emit tickCompleted(tick);
    if (hasPopulation)
        emit populationUpdated(predatorCount, producerCount);
}

long SimulationApp::currentTick() const
{
    QMutexLocker locker(&worldMutex);
    return engine ? engine->getCurrentTick() : 0L;
}

BoardSnapshot SimulationApp::collectSnapshot() const
{
    BoardSnapshot snapshot;
    QMutexLocker locker(&worldMutex);
    if (!world) return snapshot;

    snapshot.width  = world->getWidth();
    snapshot.height = world->getHeight();
    snapshot.tiles.reserve(snapshot.width * snapshot.height);

    for (int y = 0; y < world->getHeight(); ++y) {
        for (int x = 0; x < world->getWidth(); ++x) {
            if (Tile* t = world->getTile(Position(x, y))) {
                BoardTileKind kind = BoardTileKind::Normal;
                if (dynamic_cast<AbundantTile*>(t))   kind = BoardTileKind::Abundant;
                else if (dynamic_cast<PoisonTile*>(t)) kind = BoardTileKind::Poison;
                else if (dynamic_cast<ImpassableTile*>(t)) kind = BoardTileKind::Impassable;

                const EnvironmentParameters params =
                    world->getCombinedParameters(t->getPosition());
                snapshot.tiles.push_back(BoardTileSnapshot{
                    t->getPosition(), kind,
                    params.getTemperature(),
                    params.getHumidity(),
                    params.getSunlight()
                });
            }
        }
    }

    for (const auto& org : world->getOrganisms()) {
        if (!org || !org->getIsAlive()) continue;
        snapshot.organisms.push_back(BoardOrganismSnapshot{
            org->getPosition(),
            org->getLastPosition(),
            org->getColor(),
            org->getSize(),
            dynamic_cast<Predator*>(org.get()) != nullptr,
            dynamic_cast<Omnivore*>(org.get()) != nullptr,
            org->getTemperaturePreference()
        });
    }

    // Zbierz pozycje śmierci z tego ticku i wyczyść bufor w World
    snapshot.deathPositions = world->getRecentDeaths();
    world->clearRecentDeaths();

    return snapshot;
}

StatsData SimulationApp::collectStats() const
{
    StatsData data;
    QMutexLocker locker(&worldMutex);
    if (!stats || !stats->hasData()) return data;

    data.hasData          = true;
    data.ticks            = stats->getTickHistory();
    data.predatorCounts   = stats->getPredatorCountHistory();
    data.producerCounts   = stats->getProducerCountHistory();
    data.omnivoreCounts   = stats->getOmnivoreCountHistory();
    data.cryophileCounts  = stats->getCryophileCountHistory();
    data.thermophileCounts= stats->getThermophileCountHistory();
    data.defaultCounts    = stats->getDefaultCountHistory();
    data.avgSizes         = stats->getAvgSizeHistory();
    data.avgSpeeds        = stats->getAvgSpeedHistory();
    return data;
}

void SimulationApp::run()
{
    std::cout << "\n=== WATEK URUCHOMIONY ===" << std::endl;

    while (running) {
        if (paused) {
            QThread::msleep(50);
            continue;
        }

        try {
            interpolation = 0.0;

            long tick = 0;
            int  organismCount = 0, predatorCount = 0, producerCount = 0;
            bool hasPopulation = false, hasSimulation = false;

            {
                QMutexLocker locker(&worldMutex);
                hasSimulation = engine && world;

                if (hasSimulation) {
                    builder->flushSpawnQueue(world.get());
                    engine->step();

                    if (stats) {
                        stats->takeSnapshot(world.get(), engine->getCurrentTick());
                        if (stats->hasData()) {
                            const Snapshot& snap = stats->getLatestSnapshot();
                            predatorCount = snap.predatorCount;
                            producerCount = snap.producerCount;
                            hasPopulation = true;
                        }
                    }

                    tick = engine->getCurrentTick();
                    organismCount = static_cast<int>(world->getOrganisms().size());
                }
            }

            if (!hasSimulation) {
                QThread::msleep(100);
                continue;
            }

            std::cout << "TURA " << tick
                      << " | Organizmy: " << organismCount << std::endl;

            emit tickCompleted(tick);
            if (hasPopulation)
                emit populationUpdated(predatorCount, producerCount);

            int steps = tickIntervalMs / 16;
            for (int i = 0; i <= steps; i++) {
                interpolation = (double)i / (double)steps;
                QThread::msleep(16);
                if (!running || paused) break;
            }

            interpolation = 1.0;

        } catch (const std::exception& e) {
            std::cout << "BLAD w symulacji: " << e.what() << std::endl;
            running = false;
        }
    }

    std::cout << "=== WATEK ZATRZYMANY ===" << std::endl;
}

// DODANE: Dostęp do silnika symulacji (dla GUI)
SimEngine* SimulationApp::getEngine() const {
    return engine.get();
}
