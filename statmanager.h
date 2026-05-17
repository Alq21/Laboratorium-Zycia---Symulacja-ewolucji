#ifndef STATMANAGER_H
#define STATMANAGER_H

#include <vector>
#include "entity.h"
#include "temperaturepreference.h"

class World;

enum class OrganismType {
    Producer,
    Predator,
    Omnivore,
    Unknown
};

struct OrganismRecord {
    OrganismType          type;
    Color                 color;
    int                   size;
    int                   speed;
    TemperaturePreference tempPref = TemperaturePreference::Default;
};

struct Snapshot {
    long                        tick;
    std::vector<OrganismRecord> organisms;

    int predatorCount    = 0;
    int producerCount    = 0;
    int omnivoreCount    = 0;

    int cryophileCount   = 0;
    int thermophileCount = 0;
    int defaultCount     = 0;

    double avgSize  = 0.0;
    double avgSpeed = 0.0;
};

class StatManager {
public:
    StatManager() = default;
    ~StatManager() = default;
    StatManager(const StatManager&)            = delete;
    StatManager& operator=(const StatManager&) = delete;

    // Zbieranie informacji:
    void takeSnapshot(const World* world, long tick);
    void clear();

    // Dostep dla UI:
    bool hasData() const;
    const Snapshot& getLatestSnapshot() const;
    const std::vector<Snapshot>& getHistory() const;

    // Historyczne liczniki populacji:
    std::vector<int> getPredatorCountHistory()    const;
    std::vector<int> getProducerCountHistory()    const;
    std::vector<int> getOmnivoreCountHistory()    const;

    // Historyczne liczniki preferencji temperatury:
    std::vector<int> getCryophileCountHistory()   const;
    std::vector<int> getThermophileCountHistory() const;
    std::vector<int> getDefaultCountHistory()     const;

    // Historyczne srednie:
    std::vector<double> getAvgSizeHistory()  const;
    std::vector<double> getAvgSpeedHistory() const;

    // Historyczne ticki (os X wykresow):
    std::vector<long> getTickHistory() const;

    const std::vector<OrganismRecord>& getLatestOrganisms() const;

private:
    std::vector<Snapshot> history;
};

#endif // STATMANAGER_H
