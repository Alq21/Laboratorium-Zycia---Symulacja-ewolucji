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

    processMovementPlanning();
    processMovementExecution();
    processInteractions();
    processTileEffects();
    processEnergy();
    processReproduction();
    //jak dodasz stat manager co nie był dodany to sobie odkomentujesz
    // cleanup();

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
    for(const auto& organism : world->getOrganisms())
    {
        if(organism->getIsAlive() && organism->canReproduce())
        {
            auto baby = organism->reproduce();
            if(baby)
            {
                newborns.push_back(std::move(baby));
            }
        }
    }
    for(auto& child : newborns)
    {
        world->addOrganism(std::move(child));
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