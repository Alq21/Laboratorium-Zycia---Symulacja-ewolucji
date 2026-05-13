#ifndef SIMULATIONBUILDER_H
#define SIMULATIONBUILDER_H

#include <memory>
#include <queue>
#include <QString>
#include "worldconfig.h"

class World;
class Organism;

class SimulationBuilder {
public:
    SimulationBuilder() = default;
    ~SimulationBuilder() = default;

    SimulationBuilder(const SimulationBuilder&) = delete;
    SimulationBuilder& operator=(const SimulationBuilder&) = delete;

    WorldConfig loadConfig(const QString& path);
    std::unique_ptr<World> buildWorld(const WorldConfig& config);
    void queueSpawn(int x, int y, const QString& type);
    void flushSpawnQueue(World* world);

    QString lastError() const { return error; }
    bool    hasError()  const { return !error.isEmpty(); }

private:
    std::queue<std::unique_ptr<Organism>> spawnQueue;
    QString error;
    std::unique_ptr<Organism> makeOrganism(const OrganismSpawnConfig& cfg);
};

#endif // SIMULATIONBUILDER_H
