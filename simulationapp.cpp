#include "simulationapp.h"
#include "simulationbuilder.h"
#include "simengine.h"
#include "statmanager.h"
#include "world.h"
#include "organism.h"
#include "tile.h"

#include <QTimer>
#include <iostream>

SimulationApp::SimulationApp(QObject* parent)
    : QObject(parent)
    , builder(std::make_unique<SimulationBuilder>())
{
    tickTimer = new QTimer(this);
      tickIntervalMs = 600;
    tickTimer->setInterval(tickIntervalMs);
    connect(tickTimer, &QTimer::timeout, this, &SimulationApp::onTick);
}

SimulationApp::~SimulationApp() {
    tearDown();
}

bool SimulationApp::loadFromFile(const QString& path) {
    error.clear();
    tearDown();

    WorldConfig config = builder->loadConfig(path);
    if (builder->hasError()) {
        error = builder->lastError();
        emit errorOccurred(error);
        return false;
    }

    world  = builder->buildWorld(config);
    stats  = std::make_unique<StatManager>();
    engine = std::make_unique<SimEngine>(world.get(), stats.get());

    return true;
}

void SimulationApp::setTickInterval(int ms) {
    tickIntervalMs = ms;
    tickTimer->setInterval(ms);
}

void SimulationApp::start() {
    if (!world || !engine) return;
    if (running) return;

    engine->resume();
    running = true;
    paused  = false;
    tickTimer->start(tickIntervalMs);
    emit simulationStarted();
}

void SimulationApp::pause() {
    if (!running || paused) return;
    engine->pause();
    tickTimer->stop();
    paused = true;
    emit simulationPaused();
}

void SimulationApp::resume() {
    if (!running || !paused) return;
    engine->resume();
    tickTimer->start(tickIntervalMs);
    paused = false;
    emit simulationResumed();
}

void SimulationApp::stop() {
    if (!running) return;
    tickTimer->stop();
    engine->pause();
    running = false;
    paused  = false;
    emit simulationStopped();
}

void SimulationApp::spawnEntity(int x, int y, const QString& type) {
    if (!builder || !world) return;
    builder->queueSpawn(x, y, type);
    if (builder->hasError()) {
        error = builder->lastError();
        emit errorOccurred(error);
    }
}

bool SimulationApp::isRunning() const { return running; }
bool SimulationApp::isPaused()  const { return paused;  }

long SimulationApp::currentTick() const {
    return engine ? engine->getCurrentTick() : 0L;
}

std::vector<Entity*> SimulationApp::collectEntities() const {
    std::vector<Entity*> result;
    if (!world) return result;

    // === DODAJ DEBUG ===
    std::cout << "\n=== COLLECT ENTITIES ===" << std::endl;

    // Zbierz płytki
    for (int y = 0; y < world->getHeight(); ++y) {
        for (int x = 0; x < world->getWidth(); ++x) {
            if (Tile* t = world->getTile(Position(x, y))) {
                result.push_back(t);
            }
        }
    }

    std::cout << "Tiles collected: " << result.size() << std::endl;

    // Zbierz organizmy
    int orgCount = 0;
    for (const auto& org : world->getOrganisms()) {
        if (org->getIsAlive()) {
            result.push_back(org.get());
            orgCount++;
            std::cout << "  Organism #" << orgCount << " at ("
                      << org->getPosition().x << ","
                      << org->getPosition().y << ")" << std::endl;
        }
    }

    std::cout << "Organisms collected: " << orgCount << std::endl;
    std::cout << "Total entities: " << result.size() << std::endl;

    return result;
}

void SimulationApp::onTick() {
    if (!engine || !world) return;

    builder->flushSpawnQueue(world.get());

    engine->step();

    if (stats)
        stats->takeSnapshot(world.get(), engine->getCurrentTick());

   emit tickCompleted(engine->getCurrentTick());
    emitPopulationUpdate();
}

void SimulationApp::tearDown() {
    tickTimer->stop();
    engine.reset();
    stats.reset();
    world.reset();
    running = false;
    paused  = false;
}

void SimulationApp::emitPopulationUpdate() {
    if (!stats || !stats->hasData()) return;
    const Snapshot& snap = stats->getLatestSnapshot();
    emit populationUpdated(snap.predatorCount, snap.producerCount);
}
