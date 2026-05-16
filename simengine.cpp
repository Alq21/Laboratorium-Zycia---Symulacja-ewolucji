#include "simengine.h"
#include "cryophile.h"
#include "predator.h"
#include "producer.h"
#include "thermophile.h"
#include "world.h"
#include "organism.h"
#include "tile.h"
#include <iostream>
// #include "statmanager.h"
// #include "producer.h"
// #include "predator.h"

SimEngine::SimEngine(World* w, StatManager* stats)
    : world(w),
    statistics(stats),
    currentTick(0)
{
    status.setState(SimulationState::RUNNING);
}

void SimEngine::step()
{
    // Sprawdzamy stan przez obiekt status
    if (status.getState() != SimulationState::RUNNING)
        return;

    processEnergy();              // Najpierw energia i akcje
    processMovementPlanning();    // Planowanie ruchu
    processMovementExecution();   // Wykonanie ruchu
    processInteractions();        // Interakcje (polowanie)
    processTileEffects();         // Efekty płytek
    processReproduction();        // Rozmnażanie
    
    world->removeDead();
    currentTick++;
    std::cout << "Step completed, tick now: " << currentTick << std::endl;
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
        if (!organism) continue;  // <- DODAJ
        if (!organism->getIsAlive()) continue;  // <- DODAJ

        organism->planMove(world);
    }
}

void SimEngine::processMovementExecution()
{
    for (auto& organism : world->getOrganisms()) {
        if (!organism) continue;  // <- DODAJ
        if (!organism->getIsAlive()) continue;  // <- DODAJ

        organism->executeMovement(world);
    }
}

void SimEngine::processInteractions()
{
    const auto& organisms = world->getOrganisms();
    for(const auto& organism : organisms)
    {
        if(!organism->getIsAlive()) continue;

        Organism* other = world->getOrganismAt(organism->getPosition());
        if(other && other != organism.get())
        {
            organism->onInteract(other);
        }
    }
}

void SimEngine::processTileEffects()
{
    for(const auto& organism : world->getOrganisms())
    {if (!organism) continue;
        if(!organism->getIsAlive()) continue;

        Tile* tile = world->getTile(organism->getPosition());
        if(tile)
        {
            tile->applyEffect(organism.get());
        }
    }
}

void SimEngine::processEnergy()
{
    for(const auto& organism : world->getOrganisms())
    {
        if(organism->getIsAlive())
        {
            organism->onTick(world);
        }
    }
}


//to musze sie zastanowić czy ma sens, ale liczenie populacji przyda sie do statystyk
void SimEngine::processReproduction()
{
    std::vector<std::unique_ptr<Organism>> newborns;

    // liczy populacje według typu
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



    // progi
    bool totalOverpop = totalAlive > 80;
    bool producerOverpop = producers > 25;
    bool predatorOverpop = predators > 12;
    bool thermoOverpop = thermophiles > 15;
    bool cryoOverpop = cryophiles > 15;


    for (const auto& organism : world->getOrganisms()) {
        if (!organism || !organism->getIsAlive()) continue;


        if (dynamic_cast<Producer*>(organism.get()) && producerOverpop) {
            organism->setEnergy(organism->getEnergy() - 7.0);
            std::cout << "Producer overpop penalty!" << std::endl;
        }
        else if (dynamic_cast<Predator*>(organism.get()) && predatorOverpop) {
            organism->setEnergy(organism->getEnergy() - 10.0);
            std::cout << "Predator overpop penalty!" << std::endl;
        }
        else if (dynamic_cast<Thermophile*>(organism.get()) && thermoOverpop) {
            organism->setEnergy(organism->getEnergy() - 7.0);
        }
        else if (dynamic_cast<Cryophile*>(organism.get()) && cryoOverpop) {
            organism->setEnergy(organism->getEnergy() - 7.0);
        }


        if (totalOverpop) {
            organism->setEnergy(organism->getEnergy() - 15.0);
        }


        if (organism->canReproduce()) {
            auto child = organism->reproduce();
            if (child) {
                newborns.push_back(std::move(child));
            }
        }
    }

    // Dodaj nowe organizmy
    for (auto& newborn : newborns) {
        if (newborn) {
            world->addOrganism(std::move(newborn));
        }
    }
}

// void SimEngine::cleanup()
// {
//     world->removeDead();

//     if(statistics)
//     {
//         int predatorCount = 0;
//         int producerCount = 0;

//         for(const auto& organism : world->getOrganisms())
//         {
//             if(dynamic_cast<predator*>(organism.get()))
//                 predatorCount++;
//             else if(dynamic_cast<producer*>(organism.get()))
//                 producerCount++;
//         }

//         statistics->addSnapshot(predatorCount, producerCount);
//     }
// }

World* SimEngine::getWorld() const { return world; }

long SimEngine::getCurrentTick() const { return currentTick; }

SimulationStatus SimEngine::getStatus() const { return status; }

bool SimEngine::isPaused() const { return status.isPaused(); }