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

    energy -= 1.0;
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

bool Producer::canReproduce() const {
    return isAlive && (energy >= reproductionThreshold);
}

std::unique_ptr<Organism> Producer::reproduce() {
    if (!canReproduce()) {
        return nullptr;
    }

    energy -= (reproductionThreshold / 2.0);

    int childGeneration = generation + 1;
    Position childPos = {position.x + 1, position.y + 1};

    Color childColor = color;
    double childMaxEn = maxEnergy;
    int childSize = size;

    if (childGeneration % 10 == 0) {
        childMaxEn += 10.0;
        childSize += 1;
        // std::max z biblioteki <algorithm> dba o to, by kolor nie spadł poniżej zera
        childColor = Color{color.r, std::max(0, color.g - 20), color.b};
    } else {
        // std::min zapobiega przekroczeniu wartości 255
        childColor.g = std::min(255, color.g + 2);
    }

    return std::make_unique<Producer>(childPos, childColor, childMaxEn * 0.4, childMaxEn, childSize, speed, maxActionPoints, childGeneration, preferredTemperature);
}
void Producer::photosynthesize(double amount) {
    if (isAlive) {
        setEnergy(energy + amount);
        if (energy > maxEnergy) {
            energy = maxEnergy;
        }
    }
}