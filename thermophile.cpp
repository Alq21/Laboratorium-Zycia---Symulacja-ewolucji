#include "thermophile.h"

#include "environmentparameters.h"
#include "thermophile.h"
#include "world.h"
#include <cmath>

Thermophile::Thermophile(Position pos, Color col, double startEnergy, double maxEn, int startSize, int startSpeed, int maxAP, int gen)
    : Producer(pos, col, startEnergy, maxEn, startSize, startSpeed, maxAP, gen, 40.0) {
}

void Thermophile::onTick(World* world) {
    if (!isAlive) return;

    // POBIERA ZSUMOWANE PARAMETRY ŚRODOWISKA (Globalne + Lokalne kafelka)
    EnvironmentParameters currentConditions = world->getCombinedParameters(position);

    double currentTemp = currentConditions.getTemperature();
    double tempDifference = std::abs(currentTemp - preferredTemperature);

    if (tempDifference <= 10.0) {
        setEnergy(energy + 5.0);
    } else {
        setEnergy(energy - (tempDifference * 0.2));
    }

    // Density-dependent mortality
    int myPopulation = world->countPopulation<Thermophile>();
    double densityPenalty = 0.0;
    
    if (myPopulation > 30) {
        densityPenalty = (myPopulation - 30) * 0.05;
    }
    
    if (densityPenalty > 0) {
        setEnergy(energy - densityPenalty);
    }

    Producer::onTick(world);
}

std::unique_ptr<Organism> Thermophile::reproduce() {
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

    // Pełny system ewolucji
    if (childGen % 10 == 0) {
        childMaxEn += 15.0;
        childSize += 2;
        childSpeed += 1;
        childColor.r = std::min(255, childColor.r + 40);
        childColor.b = std::max(0, childColor.b - 20);
    } else {
        childColor.r = std::min(255, childColor.r + 2);
    }

    return std::make_unique<Thermophile>(
        childPos,
        childColor,
        energyGivenToChild,
        childMaxEn,
        childSize,
        childSpeed,
        maxActionPoints,
        childGen
        );
}





void Thermophile::planMove(World* world) {
    // to ogólnie bedzie działać jak dodamy płytki tak samo w crypthophile
    if (!isAlive || actionPoints <= 0) return;

    double bestTempDifference = 9999.0;
    Position bestPosition = position;

    std::vector<Position> equallyGoodPositions;

    for (int dx = -1; dx <= 1; ++dx) {
        for (int dy = -1; dy <= 1; ++dy) {
            Position checkPos = {position.x + dx, position.y + dy};

            if (world->getTile(checkPos) != nullptr) {
                EnvironmentParameters params = world->getCombinedParameters(checkPos);
                double currentTemp = params.getTemperature();

                double tempDifference = std::abs(currentTemp - preferredTemperature);

                if (tempDifference < bestTempDifference) {
                    bestTempDifference = tempDifference;
                    equallyGoodPositions.clear();
                    equallyGoodPositions.push_back(checkPos);
                } else if (tempDifference == bestTempDifference) {
                    equallyGoodPositions.push_back(checkPos);
                }
            }
        }
    }

    if (!equallyGoodPositions.empty()) {
        int randomIndex = rand() % equallyGoodPositions.size();
        plannedPosition = equallyGoodPositions[randomIndex];
    } else {
        plannedPosition = position;
    }
}
