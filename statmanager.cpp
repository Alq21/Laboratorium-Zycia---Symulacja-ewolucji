#include "statmanager.h"
#include "world.h"
#include "organism.h"
#include "predator.h"
#include "producer.h"

static OrganismType classifyOrganism(const Organism* org) {
    if (dynamic_cast<const Predator*>(org))  return OrganismType::Predator;
    if (dynamic_cast<const Producer*>(org))  return OrganismType::Producer;
    return OrganismType::Unknown;
}

void StatManager::takeSnapshot(const World* world, long tick) {
    if (!world) return;

    Snapshot snap;
    snap.tick = tick;

    for (const auto& org : world->getOrganisms()) {
        if (!org->getIsAlive()) continue;

        OrganismRecord rec;
        rec.type  = classifyOrganism(org.get());
        rec.color = org->getColor();
        rec.size  = org->getSize();
        rec.speed = org->getSpeed();

        if (rec.type == OrganismType::Predator) ++snap.predatorCount;
        if (rec.type == OrganismType::Producer) ++snap.producerCount;

        snap.organisms.push_back(rec);
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
    for (const auto& snap : history)
        result.push_back(snap.predatorCount);
    return result;
}

std::vector<int> StatManager::getProducerCountHistory() const {
    std::vector<int> result;
    result.reserve(history.size());
    for (const auto& snap : history)
        result.push_back(snap.producerCount);
    return result;
}

const std::vector<OrganismRecord>& StatManager::getLatestOrganisms() const {
    return history.back().organisms;
}
