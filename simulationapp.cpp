#include "simulationapp.h"
#include "simulationbuilder.h"
#include "simengine.h"
#include "statmanager.h"
#include "world.h"
#include "organism.h"
#include "tile.h"

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

    world = builder->buildWorld(config);
    stats = std::make_unique<StatManager>();
    engine = std::make_unique<SimEngine>(world.get(), stats.get());

    std::cout << "\n=== CONFIG LOADED ===" << std::endl;
    std::cout << "Organisms: " << world->getOrganisms().size() << std::endl;

    return true;
}

void SimulationApp::startSimulation()
{
    if (!world || !engine) {
        std::cout << "ERROR: No world or engine!" << std::endl;
        return;
    }

    if (running) {
        std::cout << "Already running!" << std::endl;
        return;
    }

    std::cout << "=== STARTING SIMULATION ===" << std::endl;
engine->resume();
    std::cout << "Engine state: " << (int)engine->getStatus().getState() << std::endl;
    running = true;
    paused = false;
    interpolation = 1.0;

    start();

    emit simulationStarted();
}

void SimulationApp::stopSimulation()
{
    if (!running) return;

    std::cout << "=== STOPPING SIMULATION ===" << std::endl;

    running = false;
    paused = false;

    wait();

    emit simulationStopped();
}

void SimulationApp::pauseSimulation()
{
    if (!running || paused) return;

    std::cout << "=== PAUSED ===" << std::endl;

    paused = true;
    emit simulationPaused();
}

void SimulationApp::resumeSimulation()
{
    if (!running || !paused) return;

    std::cout << "=== RESUMED ===" << std::endl;

    paused = false;
    emit simulationResumed();
}

void SimulationApp::stepSimulation()
{
    if (!engine || !world) return;

    std::cout << "=== STEP ===" << std::endl;

    builder->flushSpawnQueue(world.get());
    engine->step();

    if (stats)
        stats->takeSnapshot(world.get(), engine->getCurrentTick());

    emit tickCompleted(engine->getCurrentTick());
    emitPopulationUpdate();
}

long SimulationApp::currentTick() const
{
    return engine ? engine->getCurrentTick() : 0L;
}

std::vector<Entity*> SimulationApp::collectEntities() const
{
    std::vector<Entity*> result;
    if (!world) return result;

    // Zbierz płytki
    for (int y = 0; y < world->getHeight(); ++y) {
        for (int x = 0; x < world->getWidth(); ++x) {
            if (Tile* t = world->getTile(Position(x, y))) {
                result.push_back(t);
            }
        }
    }

    // Zbierz organizmy
    for (const auto& org : world->getOrganisms()) {
        if (org->getIsAlive()) {
            result.push_back(org.get());
        }
    }

    return result;
}

void SimulationApp::emitPopulationUpdate()
{
    if (!stats || !stats->hasData()) return;

    const Snapshot& snap = stats->getLatestSnapshot();
    emit populationUpdated(snap.predatorCount, snap.producerCount);
}


void SimulationApp::run()
{
    std::cout << "\n=== THREAD STARTED ===" << std::endl;

    while (running) {

        if (paused) {
            QThread::msleep(50);
            continue;
        }

        if (engine && world) {
            try {
                interpolation = 0.0;

                builder->flushSpawnQueue(world.get());
                engine->step();

                if (stats)
                    stats->takeSnapshot(world.get(), engine->getCurrentTick());

                std::cout << "TICK " << engine->getCurrentTick()
                          << " | Organisms: " << world->getOrganisms().size() << std::endl;

                emit tickCompleted(engine->getCurrentTick());
                emitPopulationUpdate();

                int steps = tickIntervalMs / 16;
                for (int i = 0; i <= steps; i++) {
                    interpolation = (double)i / (double)steps;
                    QThread::msleep(16);
                    if (!running || paused) break;
                }

                interpolation = 1.0;

            } catch (const std::exception& e) {
                std::cout << "ERROR in simulation: " << e.what() << std::endl;
                running = false;
            }
        }
        else {
            std::cout << "WARNING: No engine or world!" << std::endl;
            QThread::msleep(100);
        }
    }

    std::cout << "=== THREAD STOPPED ===" << std::endl;
}
