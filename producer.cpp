#include "producer.h"
#include "world.h"
#include <stdlib.h>

Producer::Producer(Position pos, Color col, double startEnergy, double maxEn, int size, int speed, int maxAP, int gen, double prefTemp)
    : Organism(pos, col, startEnergy, maxEn, size, speed, maxAP, gen),
    preferredTemperature(prefTemp),
    reproductionThreshold(maxEn * 0.8) {
}

void Producer::onTick(World* world) {
    if (!isAlive) {
        return;
    }

    // Podstawowy koszt życia
    double baseCost = 1.0;
    
    // Density-dependent mortality - większa populacja = większa śmiertelność
    int myPopulation = world->countPopulation<Producer>();
    double densityPenalty = 0.0;
    
    if (myPopulation > 30) {
        densityPenalty = (myPopulation - 30) * 0.05; // Kara rośnie liniowo
    }
    
    energy -= (baseCost + densityPenalty);
    actionPoints = maxActionPoints;

    if (energy <= 0.0) {
        die();
    }
}

void Producer::planMove(World* world) {
    if (!isAlive || actionPoints <= 0) {
        return;
    }

    int dx = (rand() % 3) - 1;
    int dy = (rand() % 3) - 1;

    plannedPosition = Position{position.x + dx, position.y + dy};
}

void Producer::onInteract(Entity* other) {
    // Producenci nie atakują innych jednostek, ignorują interakcje
}


std::unique_ptr<Organism> Producer::reproduce() {
    // Niższy próg reprodukcji - 60% zamiast 80%
    if (!isAlive || energy < (maxEnergy * 0.6)) {
        return nullptr;
    }

    double energyGivenToChild = energy / 2.0;
    setEnergy(energy - energyGivenToChild);

    int childGen = generation + 1;
    Position childPos = {position.x + 1, position.y + 1};

    double childMaxEn = maxEnergy;
    int childSize = size;
    int childSpeed = speed;
    Color childColor = color;

    // Mutacja co 10 pokoleń
    if (childGen % 10 == 0) {
        childMaxEn += 15.0;
        childSize += 2;
        childSpeed += 1;
        childColor.r = std::min(255, childColor.r + 30);
    } else {
        childColor.g = std::max(50, childColor.g - 5);
    }

    return std::make_unique<Producer>(
        childPos,
        childColor,
        energyGivenToChild,
        childMaxEn,
        childSize,
        childSpeed,
        maxActionPoints,
        childGen,
        preferredTemperature
        );
}
void Producer::photosynthesize(double amount) {
    if (isAlive) {
        setEnergy(energy + amount);
        if (energy > maxEnergy) {
            energy = maxEnergy;
        }
    }
}
