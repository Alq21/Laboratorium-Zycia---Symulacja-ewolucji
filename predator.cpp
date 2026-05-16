#include "predator.h"
#include "world.h"
#include <cstdlib>

Predator::Predator(Position pos, Color col, double startEnergy, double maxEn, int size, int speed, int maxAP, int gen, int vision)
    : Organism(pos, col, startEnergy, maxEn, size, speed, maxAP, gen), visionRange(vision) {
}
Position Predator::findNearestPrey(World* world) {
    Position bestPos = {-1, -1};

    double minDistanceSq = (visionRange * visionRange) + 1.0;

    for (const auto& org : world->getOrganisms()) {

        if (org->getIsAlive() && org.get() != this && !dynamic_cast<Predator*>(org.get())) {

            Position pPos = org->getPosition();

            double dx = pPos.x - this->position.x;
            double dy = pPos.y - this->position.y;
            double distSq = (dx * dx) + (dy * dy);


            if (distSq <= (visionRange * visionRange) && distSq < minDistanceSq) {
                minDistanceSq = distSq;
                bestPos = pPos;
            }
        }
    }

    return bestPos;
}
void Predator::onTick(World* world) {
    if (!isAlive) return;

    // Density-dependent mortality dla drapieżników
    int myPopulation = world->countPopulation<Predator>();
    double densityPenalty = 0.0;
    
    // Drapieżnicy cierpią bardziej przy przeludnieniu
    if (myPopulation > 15) {
        densityPenalty = (myPopulation - 15) * 0.1;
    }

    // Stosujemy podstawowy onTick z Organism
    Organism::onTick(world);
    
    // Dodatkowa kara za przeludnienie
    if (densityPenalty > 0) {
        setEnergy(energy - densityPenalty);
    }
}

void Predator::planMove(World* world) {
    if (!isAlive || actionPoints <= 0) {
        return;
    }


    Position preyPos = findNearestPrey(world);

    if (preyPos.x != -1) {
        int dx = (preyPos.x > position.x) ? 1 : ((preyPos.x < position.x) ? -1 : 0);
        int dy = (preyPos.y > position.y) ? 1 : ((preyPos.y < position.y) ? -1 : 0);
        plannedPosition = Position{position.x + dx, position.y + dy};
    } else {
        int dx = (rand() % 3) - 1;
        int dy = (rand() % 3) - 1;
        plannedPosition = Position{position.x + dx, position.y + dy};
    }
}

void Predator::onInteract(Entity* other) {
    if (!isAlive) {
        return;
    }

    Organism* prey = dynamic_cast<Organism*>(other);

    if (prey && prey->getIsAlive() && !dynamic_cast<Predator*>(prey)) {
        hunt(prey);
    }
}

void Predator::hunt(Organism* target) {
    if (target && target->getIsAlive()) {
        double stolenEnergy = target->getEnergy() / 2.0;
        target->setEnergy(target->getEnergy() - stolenEnergy);
        this->setEnergy(this->energy + stolenEnergy);
    }
}

bool Predator::canReproduce() const {
    return isAlive && (energy >= 150.0);
}

std::unique_ptr<Organism> Predator::reproduce() {
    if (!canReproduce()) {
        return nullptr;
    }

    double energyGivenToChild = energy / 2.0;
    setEnergy(energy - energyGivenToChild);

    int childGen = generation + 1;
    Position childPos = {position.x - 1, position.y - 1};

    double childMaxEn = maxEnergy;
    int childSize = size;
    int childSpeed = speed;
    int childVision = visionRange;
    Color childColor = color;

    if (childGen % 10 == 0) {
        childMaxEn += 20.0;
        childSize += 3;
        childSpeed += 2;
        childVision += 2;
        childColor.b = std::min(255, childColor.b + 50);
    } else {
        childColor.r = std::max(100, childColor.r - 5);
    }

    return std::make_unique<Predator>(
        childPos,
        childColor,
        energyGivenToChild,
        childMaxEn,
        childSize,
        childSpeed,
        maxActionPoints,
        childGen,
        childVision
        );
}
