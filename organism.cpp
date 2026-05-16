#include "organism.h"
#include "world.h"
#include <stdlib.h>
#include "tile.h"
#include <QDebug>

Organism::Organism(Position pos, Color col, double startEnergy, double maxEn, int size, int speed, int maxAP, int gen)
    : Entity(pos, col), energy(startEnergy), maxEnergy(maxEn), isAlive(true), size(size), speed(speed),
    actionPoints(maxAP), maxActionPoints(maxAP), plannedPosition(pos), generation(gen),
    previousPosition(pos), isMoving(false) {
}

void Organism::setEnergy(double newEnergy) {
    energy = newEnergy;
    if (energy > maxEnergy) {
        energy = maxEnergy;
    }
    if (energy <= 0.0) {
        energy = 0.0;
        die();
    }
}
void Organism::die() {
    isAlive = false;
    energy=0;


}

void Organism::onTick(World* world) {
    if (!isAlive) return;

 setEnergy(energy - 10.0);
    if (energy <= 0) {
        die();
    }


    double energyRatio = energy / maxEnergy;
    actionPoints = std::max(1, static_cast<int>(speed * energyRatio));
}



void Organism::planMove(World* world) {
    if (!isAlive || actionPoints <= 0) return;

    //  zaplanuj ruch w losowym kierunku


    int newX = position.x + (rand() % 3 - 1);
    int newY = position.y + (rand() % 3 - 1);

    plannedPosition = Position(newX, newY);}

void Organism::onInteract(Entity* other) {

}


bool Organism::canReproduce() const {
  return isAlive && (energy >= (maxEnergy * 0.8));
}
void Organism::executeMovement(World* world) {
    if (!isAlive || actionPoints <= 0) return;

    if (position.x != plannedPosition.x || position.y != plannedPosition.y) {
        // Sprawdzenie czy pozycja jest w granicach mapy
        if (plannedPosition.x < 0 || plannedPosition.x >= world->getWidth() ||
            plannedPosition.y < 0 || plannedPosition.y >= world->getHeight()) {
            plannedPosition = position;
            isMoving = false;
            return;
        }

        // Sprawdzenie czy kafelek jest przechodzalny
        Tile* targetTile = world->getTile(plannedPosition);
        if (targetTile && !targetTile->isTraversable()) {
            plannedPosition = position;
            isMoving = false;
            return;
        }

        // Ruch jest dozwolony - zapisujemy poprzednią pozycję dla animacji
        previousPosition = position;
        position = plannedPosition;
        isMoving = true;
        actionPoints--;

        // Koszt ruchu
        setEnergy(energy - 1.0);

        // Natychmiastowe efekty tile po wejściu
        if (targetTile) {
            targetTile->applyEffect(this);
        }
    } else {
        isMoving = false;
    }
}

bool Organism::getIsAlive() const { return isAlive; }
double Organism::getEnergy() const { return energy; }
int Organism::getGeneration() const { return generation; }
int Organism::getSize() const { return size; }
Color Organism::getColor() const { return color; }
int Organism::getSpeed() const { return speed; }
