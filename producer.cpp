#include "producer.h"
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
    energy -= 1.0;
    actionPoints = maxActionPoints;

    // Fotosynteza - producenci zyskują energię z otoczenia
    photosynthesize(5.0);

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
    Color childColor = color;

    // Mutacje z szansą 20%
    if (rand() % 100 < 20) {
        // Losowa mutacja parametrów
        int mutationType = rand() % 4;
        switch(mutationType) {
        case 0: // Większa pojemność energii
            childMaxEn += 10.0 + (rand() % 10);
            childColor.r = std::min(255, childColor.r + 20);
            break;
        case 1: // Większy rozmiar
            childSize += 1;
            childColor.g = std::min(255, childColor.g + 15);
            break;
        case 2: // Większa szybkość
            childSpeed += 1;
            childColor.b = std::min(255, childColor.b + 15);
            break;
        case 3: // Zmiana preferowanej temperatury
            preferredTemperature += ((rand() % 10) - 5);
            childColor.r = std::max(50, childColor.r - 10);
            childColor.g = std::min(255, childColor.g + 20);
            break;
        }
    } else {
        // Drobne zmiany kolorów bez mutacji
        childColor.g = std::max(50, childColor.g - 3);
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