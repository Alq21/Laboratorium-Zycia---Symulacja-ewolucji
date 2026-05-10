#ifndef SIMENGINE_H
#define SIMENGINE_H

#include "simulationstatus.h"

class World;
class StatManager;

class SimEngine {
private:
    World* world;
    StatManager* statistics;

    long currentTick;
    SimulationStatus status;

    // Fazy symulacji:
    void processMovementPlanning();
    void processMovementExecution();
    void processInteractions();
    void processTileEffects();
    void processEnergy();
    void processReproduction();
    void cleanup();

public:
    SimEngine(World* w, StatManager* stats);
    ~SimEngine() = default;

    // Sterowanie:
    void step();
    void pause();
    void resume();

    // Gettery:
    World* getWorld() const;
    long getCurrentTick() const;
    SimulationStatus getStatus() const;
    bool isPaused() const;

    // Rule of 5:
    SimEngine(const SimEngine&) = delete;
    SimEngine& operator=(const SimEngine&) = delete;
};

#endif // SIMENGINE_H