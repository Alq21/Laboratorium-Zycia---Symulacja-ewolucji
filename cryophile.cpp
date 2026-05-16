#include "cryophile.h"
#include "world.h"
#include <cmath>

Cryophile::Cryophile(Position pos, Color col, double startEnergy, double maxEn, int startSize, int startSpeed, int maxAP, int gen)
    : Producer(pos, col, startEnergy, maxEn, startSize, startSpeed, maxAP, gen, -5.0) {
}

void Cryophile::onTick(World* world) {
    if (!isAlive) return;

    EnvironmentParameters currentConditions = world->getCombinedParameters(position);

    double currentTemp = currentConditions.getTemperature();
    double tempDifference = std::abs(currentTemp - preferredTemperature);

    if (tempDifference <= 10.0) {
        setEnergy(energy + 5.0);
    } else {
        setEnergy(energy - (tempDifference * 0.2));
    }

    int myPopulation = world->countPopulation<Cryophile>();
    double densityPenalty = 0.0;
    
    if (myPopulation > 30) {
        densityPenalty = (myPopulation - 30) * 0.05;
    }
    
    if (densityPenalty > 0) {
        setEnergy(energy - densityPenalty);
    }

    Producer::onTick(world);
}



void Cryophile::planMove(World* world) {
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

std::unique_ptr<Organism> Cryophile::reproduce() {
    // Niższy próg reprodukcji - 60% zamiast 80%
    if (!isAlive || energy < (maxEnergy * 0.6)) {
        return nullptr;
    }




        // Koszt
        double childEnergy = energy * 0.4;
        energy -= childEnergy;

        Position childPos = position;
        childPos.x += 1;

        // mutacje
        double childMaxEn = maxEnergy;
        int childSize = size;
        Color childColor = color;

        if (rand() % 100 < 30) {
            // Mutacja koloru
            childColor.r = 50 + (rand() % 100);
            childColor.g = 150 + (rand() % 106);
            childColor.b = 200 + (rand() % 56);

            // Mutacja rozmiaru
            int sizeChange = (rand() % 5) - 2;
            childSize += sizeChange;
            if (childSize < 1) childSize = 1;
            if (childSize > 6) childSize = 6;

            // Mutacja energii
            int energyChange = (rand() % 80) - 40;
            childMaxEn += energyChange;
            if (childMaxEn < 100) childMaxEn = 100;
            if (childMaxEn > 450) childMaxEn = 450;


        }

        return std::make_unique<Cryophile>(
            childPos,
            childColor,
            childEnergy,
            childMaxEn,
            childSize,
            speed,
            maxActionPoints,
            generation + 1
            );

}
