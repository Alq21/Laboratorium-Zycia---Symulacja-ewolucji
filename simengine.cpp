#include "simengine.h"
#include "cryophile.h"
#include "predator.h"
#include "producer.h"
#include "thermophile.h"
#include "world.h"
#include "organism.h"
#include "tile.h"

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
    for (auto& organism : world->getOrganisms()) {
        if (!organism) continue;
        if (!organism->getIsAlive()) continue;

        try {
            organism->planMove(world);
        } catch (...) {

        }
    }
}

void SimEngine::processMovementExecution()
{
    for (auto& organism : world->getOrganisms()) {
        if (!organism) continue;
        if (!organism->getIsAlive()) continue;

        try {
            organism->executeMovement(world);
        } catch (...){

        }
    }
}

void SimEngine::processInteractions()
{
    for (const auto& organism : world->getOrganisms()) {
        if (!organism || !organism->getIsAlive()) continue;

        Predator* pred = dynamic_cast<Predator*>(organism.get());
        if (!pred) continue;

        try {
            Position pos = pred->getPosition();
            int vision = pred->getVision();

            Organism* closestPrey = nullptr;
            double closestDist = 999999.0;

            for (const auto& other : world->getOrganisms()) {
                if (!other || !other->getIsAlive()) continue;
                if (other.get() == pred) continue;
                if (dynamic_cast<Predator*>(other.get())) continue;

                Position otherPos = other->getPosition();
                double dist = std::abs(pos.x - otherPos.x) + std::abs(pos.y - otherPos.y);

                if (dist <= vision && dist < closestDist) {
                    closestDist = dist;
                    closestPrey = other.get();
                }
            }

            if (closestPrey && closestDist <= 1.5) {
                pred->hunt(closestPrey);
            }
        } catch (...) {

        }
    }
}

void SimEngine::processTileEffects()
{
    for (const auto& organism : world->getOrganisms()) {
        if (!organism) continue;
        if (!organism->getIsAlive()) continue;

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
        if (!organism) continue;
        if (organism->getIsAlive()) {
            try {
                organism->onTick(world);
            } catch (...) {

            }
        }
    }
}

void SimEngine::processReproduction()
{
    std::vector<std::unique_ptr<Organism>> newborns;

    // Policz populacje
    int producers = 0, predators = 0, thermophiles = 0, cryophiles = 0;
    int totalAlive = 0;

    for (const auto& org : world->getOrganisms()) {
        if (!org || !org->getIsAlive()) continue;

        totalAlive++;

        if (dynamic_cast<Producer*>(org.get())) {
            producers++;
        } else if (dynamic_cast<Predator*>(org.get())) {
            predators++;
        } else if (dynamic_cast<Thermophile*>(org.get())) {
            thermophiles++;
        } else if (dynamic_cast<Cryophile*>(org.get())) {
            cryophiles++;
        }
    }


    if (totalAlive > 100) {
        // Zabij losowe organizmy żeby zejść do 80
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

    // Progi
    bool totalOverpop = totalAlive > 50;
    bool producerOverpop = producers > 25;
    bool predatorOverpop = predators > 12;
    bool thermoOverpop = thermophiles > 15;
    bool cryoOverpop = cryophiles > 15;

    // Kary za przeludnienie
    for (const auto& organism : world->getOrganisms()) {
        if (!organism || !organism->getIsAlive()) continue;

        try {
            double penalty = 0.0;

            // Kara za przeludnienie SWOJEGO typu
            if (dynamic_cast<Producer*>(organism.get()) && producerOverpop) {
                penalty += 3.0;
            }
            else if (dynamic_cast<Predator*>(organism.get()) && predatorOverpop) {
                penalty += 4.0;
            }
            else if (dynamic_cast<Thermophile*>(organism.get()) && thermoOverpop) {
                penalty += 3.0;
            }
            else if (dynamic_cast<Cryophile*>(organism.get()) && cryoOverpop) {
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

    // Dodaj nowe organizmy
    for (auto& newborn : newborns) {
        if (newborn) {
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