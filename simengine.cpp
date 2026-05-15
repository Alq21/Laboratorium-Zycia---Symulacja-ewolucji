#include "simengine.h"
#include "world.h"
#include "organism.h"
#include "tile.h"
// #include "statmanager.h"
// #include "producer.h"
// #include "predator.h"

SimEngine::SimEngine(World* w, StatManager* stats)
    : world(w),
    statistics(stats),
    currentTick(0)
{
    status.setState(SimulationState::STOPPED);
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
    for(const auto& organism : world->getOrganisms())
    {
        if(organism->getIsAlive())
        {
            organism->planMove(world);
        }
    }
}

void SimEngine::processMovementExecution()
{
    for(const auto& organism : world->getOrganisms())
    {
        if(organism->getIsAlive())
        {
            organism->executeMovement(world);
        }
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
    {
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



    void SimEngine::processReproduction()
    {
        std::vector<std::unique_ptr<Organism>> newborns;

        // Policz żywe organizmy
        int aliveCount = 0;
        for(const auto& org : world->getOrganisms()) {
            if(org->getIsAlive()) aliveCount++;
        }

        // Jeśli przeludnienie (>50 organizmów), zwiększ śmiertelność
        bool overpopulation = aliveCount > 20;

        for(const auto& organism : world->getOrganisms())
        {
            if(!organism->getIsAlive()) continue;

            // Przeludnienie = dodatkowa utrata energii
            if (overpopulation) {
                organism->setEnergy(organism->getEnergy() - 2.0);
            }

            if(organism->canReproduce())
            {
                auto child = organism->reproduce();
                if(child) {
                    newborns.push_back(std::move(child));
                }
            }
        }

        for(auto& newborn : newborns) {
            world->addOrganism(std::move(newborn));
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