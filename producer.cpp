#include "producer.h"
#include "world.h"
#include <stdlib.h>

Producer::Producer(Position pos, Color col, double startEnergy, double maxEn, int size, int speed, int maxAP, int gen, double prefTemp)
    : Organism(pos, col, startEnergy, maxEn, size, speed, maxAP, gen),
    preferredTemperature(prefTemp),
    reproductionThreshold(maxEn * 0.8) {
}

void Producer::onTick(World* world) {
    if (!isAlive) return;


    setEnergy(energy - 1);

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


std::unique_ptr<Organism> Producer::reproduce()
{
    if (!canReproduce()) return nullptr;

    double childEnergy = energy * 0.4;
    energy -= childEnergy;

    Position childPos = position;
    childPos.x += 1;


    double childMaxEn = maxEnergy;
    int childSize = size;
    Color childColor = color;
    double childPrefTemp = preferredTemperature;


    if (rand() % 100 < 30) {
        // Mutacja koloru
        childColor.r += (rand() % 100) - 50;
        childColor.g += (rand() % 100) - 50;
        childColor.b += (rand() % 60) - 30;


        if (childColor.r < 80) childColor.r = 80;
        if (childColor.r > 255) childColor.r = 255;
        if (childColor.g < 150) childColor.g = 150;
        if (childColor.g > 255) childColor.g = 255;
        if (childColor.b < 20) childColor.b = 20;
        if (childColor.b > 120) childColor.b = 120;

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

    return std::make_unique<Producer>(
        childPos,
        childColor,
        childEnergy,
        childMaxEn,
        childSize,
        speed,
        maxActionPoints,
        generation + 1,
        childPrefTemp
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
