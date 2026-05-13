#ifndef STATMANAGER_H
#define STATMANAGER_H

#include <vector>
#include <string>
#include "entity.h" // Color

class World;

enum class OrganismType {
    Producer,
    Predator,
    Unknown
};

struct OrganismRecord {
    OrganismType type;
    Color        color;
    int          size;
    int          speed;
};

struct Snapshot {
    long                      tick;
    std::vector<OrganismRecord> organisms;
    int predatorCount = 0;
    int producerCount = 0;
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

    //Dostęp dla UI:

    bool hasData() const;
    const Snapshot& getLatestSnapshot() const;
    const std::vector<Snapshot>& getHistory() const;
    std::vector<int> getPredatorCountHistory() const;
    std::vector<int> getProducerCountHistory() const;
    const std::vector<OrganismRecord>& getLatestOrganisms() const;

private:
    std::vector<Snapshot> history;
};

#endif // STATMANAGER_H
