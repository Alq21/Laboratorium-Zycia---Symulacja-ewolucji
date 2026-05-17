#include "statmanager.h"
#include "world.h"
#include "organism.h"
#include "predator.h"
#include "producer.h"
#include "omnivore.h"

static OrganismType classifyOrganism(const Organism* org) {
    if (dynamic_cast<const Predator*>(org))  return OrganismType::Predator;
    if (dynamic_cast<const Producer*>(org))  return OrganismType::Producer;
    if (dynamic_cast<const Omnivore*>(org))  return OrganismType::Omnivore;
    return OrganismType::Unknown;
}

void StatManager::takeSnapshot(const World* world, long tick) {
    if (!world) return;

    Snapshot snap;
    snap.tick = tick;

    long totalSize  = 0;
    long totalSpeed = 0;

    for (const auto& org : world->getOrganisms()) {
        if (!org->getIsAlive()) continue;

        OrganismRecord rec;
        rec.type     = classifyOrganism(org.get());
        rec.color    = org->getColor();
        rec.size     = org->getSize();
        rec.speed    = org->getSpeed();
        rec.tempPref = org->getTemperaturePreference();

        // Liczniki typu:
        if (rec.type == OrganismType::Predator) ++snap.predatorCount;
        if (rec.type == OrganismType::Producer) ++snap.producerCount;
        if (rec.type == OrganismType::Omnivore) ++snap.omnivoreCount;

        // Liczniki preferencji temperaturowej:
        switch (rec.tempPref) {
            case TemperaturePreference::Cryophile:   ++snap.cryophileCount;   break;
            case TemperaturePreference::Thermophile: ++snap.thermophileCount; break;
            default:                                 ++snap.defaultCount;     break;
        }

        totalSize  += rec.size;
        totalSpeed += rec.speed;

        snap.organisms.push_back(rec);
    }

    const int total = static_cast<int>(snap.organisms.size());
    if (total > 0) {
        snap.avgSize  = static_cast<double>(totalSize)  / total;
        snap.avgSpeed = static_cast<double>(totalSpeed) / total;
    }

    history.push_back(std::move(snap));
}

void StatManager::clear() {
    history.clear();
}

bool StatManager::hasData() const {
    return !history.empty();
}

const Snapshot& StatManager::getLatestSnapshot() const {
    return history.back();
}

const std::vector<Snapshot>& StatManager::getHistory() const {
    return history;
}

std::vector<int> StatManager::getPredatorCountHistory() const {
    std::vector<int> result;
    result.reserve(history.size());
    for (const auto& snap : history) result.push_back(snap.predatorCount);
    return result;
}

std::vector<int> StatManager::getProducerCountHistory() const {
    std::vector<int> result;
    result.reserve(history.size());
    for (const auto& snap : history) result.push_back(snap.producerCount);
    return result;
}

std::vector<int> StatManager::getOmnivoreCountHistory() const {
    std::vector<int> result;
    result.reserve(history.size());
    for (const auto& snap : history) result.push_back(snap.omnivoreCount);
    return result;
}

std::vector<int> StatManager::getCryophileCountHistory() const {
    std::vector<int> result;
    result.reserve(history.size());
    for (const auto& snap : history) result.push_back(snap.cryophileCount);
    return result;
}

std::vector<int> StatManager::getThermophileCountHistory() const {
    std::vector<int> result;
    result.reserve(history.size());
    for (const auto& snap : history) result.push_back(snap.thermophileCount);
    return result;
}

std::vector<int> StatManager::getDefaultCountHistory() const {
    std::vector<int> result;
    result.reserve(history.size());
    for (const auto& snap : history) result.push_back(snap.defaultCount);
    return result;
}

std::vector<double> StatManager::getAvgSizeHistory() const {
    std::vector<double> result;
    result.reserve(history.size());
    for (const auto& snap : history) result.push_back(snap.avgSize);
    return result;
}

std::vector<double> StatManager::getAvgSpeedHistory() const {
    std::vector<double> result;
    result.reserve(history.size());
    for (const auto& snap : history) result.push_back(snap.avgSpeed);
    return result;
}

std::vector<long> StatManager::getTickHistory() const {
    std::vector<long> result;
    result.reserve(history.size());
    for (const auto& snap : history) result.push_back(snap.tick);
    return result;
}

const std::vector<OrganismRecord>& StatManager::getLatestOrganisms() const {
    return history.back().organisms;
}
