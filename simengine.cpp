#include "simengine.h"
#include "predator.h"
#include "producer.h"
#include "omnivore.h"
#include "world.h"
#include "organism.h"
#include "tile.h"

#include <cmath>
#include <cstdlib>
#include <memory>
#include <vector>

SimEngine::SimEngine(World* w, StatManager* stats)
    : world(w),
    statistics(stats),
    currentTick(0)
{
    status.setState(SimulationState::RUNNING);
}

void SimEngine::step()
{
    if (status.getState() != SimulationState::RUNNING)
        return;

    try {
        processEnergy();
        processMovementPlanning();
        processMovementExecution();
        processInteractions();
        processTileEffects();
        processReproduction();
        world->removeDead();
        currentTick++;
    } catch (...) {
        status.setState(SimulationState::PAUSED);
    }
}

void SimEngine::pause()
{
    status.setState(SimulationState::PAUSED);
}

void SimEngine::resume()
{
    status.setState(SimulationState::RUNNING);
}

void SimEngine::processMovementPlanning()
{
    // Producers plan first; predators only see current prey positions, not producer plans.
    for (auto& organism : world->getOrganisms()) {
        if (!organism || !organism->getIsAlive()) continue;

        try {
            if (dynamic_cast<Producer*>(organism.get())) {
                organism->planMove(world);
            }
        } catch (...) {
        }
    }

    for (auto& organism : world->getOrganisms()) {
        if (!organism || !organism->getIsAlive()) continue;

        try {
            if (dynamic_cast<Predator*>(organism.get()) ||
                dynamic_cast<Omnivore*>(organism.get())) {
                organism->planMove(world);
            }
        } catch (...) {
        }
    }
}

void SimEngine::processMovementExecution()
{
    for (auto& organism : world->getOrganisms()) {
        if (!organism || !organism->getIsAlive()) continue;

        try {
            organism->executeMovement(world);
        } catch (...) {
        }
    }
}

static void processHunterInteractions(World* world, Organism* hunter, int vision, bool skipPredatorPrey)
{
    if (!world || !hunter) {
        return;
    }

    const Position pos = hunter->getPosition();
    Organism* closestPrey = nullptr;
    double closestDist = 999999.0;

    for (const auto& other : world->getOrganisms()) {
        if (!other || !other->getIsAlive()) continue;
        if (other.get() == hunter) continue;
        if (hunter->isDirectKin(other.get())) continue;
        if (skipPredatorPrey && dynamic_cast<Predator*>(other.get())) continue;

        const Position otherPos = other->getPosition();
        const double dist = std::abs(pos.x - otherPos.x) + std::abs(pos.y - otherPos.y);

        if (dist <= vision && dist < closestDist) {
            closestDist = dist;
            closestPrey = other.get();
        }
    }

    if (!closestPrey || closestDist > 1.5) {
        return;
    }

    if (Predator* predator = dynamic_cast<Predator*>(hunter)) {
        predator->attemptHunt(closestPrey);
    } else if (Omnivore* omnivore = dynamic_cast<Omnivore*>(hunter)) {
        omnivore->attemptHunt(closestPrey);
    }
}

void SimEngine::processInteractions()
{
    for (const auto& organism : world->getOrganisms()) {
        if (!organism || !organism->getIsAlive()) continue;

        try {
            if (Predator* pred = dynamic_cast<Predator*>(organism.get())) {
                processHunterInteractions(world, pred, pred->getVision(), true);
            } else if (Omnivore* omni = dynamic_cast<Omnivore*>(organism.get())) {
                processHunterInteractions(world, omni, omni->getVision(), false);
            }
        } catch (...) {
        }
    }
}

void SimEngine::processTileEffects()
{
    for (const auto& organism : world->getOrganisms()) {
        if (!organism || !organism->getIsAlive()) continue;

        try {
            Tile* tile = world->getTile(organism->getPosition());
            if (tile) {
                tile->applyEffect(organism.get());
            }
        } catch (...) {
        }
    }
}

void SimEngine::processEnergy()
{
    for (const auto& organism : world->getOrganisms()) {
        if (!organism || !organism->getIsAlive()) continue;

        try {
            organism->onTick(world);
        } catch (...) {
        }
    }
}

void SimEngine::processReproduction()
{
    std::vector<std::unique_ptr<Organism>> newborns;

    int producers = 0;
    int predators = 0;
    int thermophiles = 0;
    int cryophiles = 0;
    int totalAlive = 0;

    for (const auto& org : world->getOrganisms()) {
        if (!org || !org->getIsAlive()) continue;

        totalAlive++;

        if (dynamic_cast<Producer*>(org.get())) {
            producers++;
        } else if (dynamic_cast<Predator*>(org.get())) {
            predators++;
        }

        if (org->getTemperaturePreference() == TemperaturePreference::Thermophile) {
            thermophiles++;
        } else if (org->getTemperaturePreference() == TemperaturePreference::Cryophile) {
            cryophiles++;
        }
    }

    if (totalAlive > 100) {
        int toKill = totalAlive - 80;
        int killed = 0;

        for (auto& org : world->getOrganisms()) {
            if (!org || !org->getIsAlive()) continue;
            if (killed >= toKill) break;

            if (rand() % 2 == 0) {
                org->die();
                killed++;
            }
        }

        return;
    }

    bool totalOverpop = totalAlive > 200;
    bool producerOverpop = producers > 100;
    bool predatorOverpop = predators > 20;
    bool thermoOverpop = thermophiles > 100;
    bool cryoOverpop = cryophiles > 100;

    for (const auto& organism : world->getOrganisms()) {
        if (!organism || !organism->getIsAlive()) continue;

        try {
            double penalty = 0.0;

            if (dynamic_cast<Producer*>(organism.get()) && producerOverpop) {
                penalty += 2.0;
            }
            else if (dynamic_cast<Predator*>(organism.get()) && predatorOverpop) {
                penalty += 3.5;
            }

            if (organism->getTemperaturePreference() == TemperaturePreference::Thermophile && thermoOverpop) {
                penalty += 3.0;
            }
            else if (organism->getTemperaturePreference() == TemperaturePreference::Cryophile && cryoOverpop) {
                penalty += 3.0;
            }

            if (totalOverpop) {
                penalty += 2.0;
            }

            if (penalty > 0.0) {
                organism->setEnergy(organism->getEnergy() - penalty);
            }

            if (organism->canReproduce()) {
                auto child = organism->reproduce();
                if (child) {
                    newborns.push_back(std::move(child));
                }
            }
        } catch (...) {
        }
    }

    for (auto& newborn : newborns) {
        if (!newborn) continue;

        Position chosen = newborn->findPlaceToBreed(world);
        if (chosen.x >= 0) {
            newborn->setPosition(chosen);
            try {
                world->addOrganism(std::move(newborn));
            } catch (...) {
            }
        }
    }
}

World* SimEngine::getWorld() const { return world; }
long SimEngine::getCurrentTick() const { return currentTick; }
SimulationStatus SimEngine::getStatus() const { return status; }
bool SimEngine::isPaused() const { return status.isPaused(); }
