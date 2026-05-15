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
        setEnergy(energy + 5.0);  // zysk z adobre srodowisko
    } else {
        setEnergy(energy - (tempDifference * 0.2)); // Kara za złe środowisko
    }

    Producer::onTick(world);
}

std::unique_ptr<Organism> Thermophile::reproduce() {
    if (!canReproduce()) {
        return nullptr;
    }

    double energyGivenToChild = energy / 2.0;
    setEnergy(energy - energyGivenToChild);

    int childGen = generation + 1;
    Position childPos = {position.x + 1, position.y + 1};

    double childMaxEn = maxEnergy;
    int childSize = size;
    int childSpeed = speed;
    double childPrefTemp = preferredTemperature;
    Color childColor = color;

    // Mutacje z szansą 20%
    if (rand() % 100 < 20) {
        int mutationType = rand() % 4;
        switch(mutationType) {
        case 0: // Większa pojemność energii
            childMaxEn += 12.0 + (rand() % 10);
            childColor.r = std::min(255, childColor.r + 35);
            break;
        case 1: // Większy rozmiar
            childSize += 1;
            childColor.r = std::min(255, childColor.r + 20);
            break;
        case 2: // Większa szybkość
            childSpeed += 1;
            childColor.r = std::min(255, childColor.r + 25);
            break;
        case 3: // Wyższa preferowana temperatura
            childPrefTemp += 2.0 + (rand() % 5);
            childColor.r = std::min(255, childColor.r + 40);
            childColor.b = std::max(0, childColor.b - 20);
            break;
        }
    } else {
        // Drobne zmiany kolorów bez mutacji
        childColor.r = std::min(255, childColor.r + 2);
    }

    auto child = std::make_unique<Thermophile>(
        childPos,
        childColor,
        energyGivenToChild,
        childMaxEn,
        childSize,
        childSpeed,
        maxActionPoints,
        childGen
        );
    // Ustawiamy nową preferowaną temperaturę
    child->preferredTemperature = childPrefTemp;
    return child;
}





void Thermophile::planMove(World* world) {
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
                    equallyGoodPositions.clear(); // Czyścimy stare wyniki
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