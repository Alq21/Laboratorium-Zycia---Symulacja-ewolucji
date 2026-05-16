#include "organism.h"
#include "world.h"
#include <stdlib.h>
#include "tile.h"
#include <QDebug>

Organism::Organism(Position pos, Color col, double startEnergy, double maxEn, int size, int speed, int maxAP, int gen)
    : Entity(pos, col), energy(startEnergy), maxEnergy(maxEn), isAlive(true), size(size), speed(speed),
    actionPoints(maxAP), maxActionPoints(maxAP), plannedPosition(pos), generation(gen),
   lastPosition(pos), isMoving(false) {
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

 setEnergy(energy - 1.0);
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

        if (plannedPosition.x < 0 || plannedPosition.x >= world->getWidth() ||
            plannedPosition.y < 0 || plannedPosition.y >= world->getHeight()) {
            plannedPosition = position;
            isMoving = false;
            return;
        }

        // Sczy kafelek to nie blokada
        Tile* targetTile = world->getTile(plannedPosition);
        if (targetTile && !targetTile->isTraversable()) {
            plannedPosition = position;
            isMoving = false;
            return;
        }
        //  by organizmy sie nie nakładały ale to tak srednio działa
        Organism* occupant = world->getOrganismAt(plannedPosition);
        if (occupant != nullptr && occupant != this) {
            // Pozycja zajęta - szukaj alternatywy
            std::vector<Position> alternatives;

            for (int dy = -1; dy <= 1; dy++) {
                for (int dx = -1; dx <= 1; dx++) {
                    if (dx == 0 && dy == 0) continue;

                    Position alt = {position.x + dx, position.y + dy};

                    // Sprawdź granice
                    if (alt.x < 0 || alt.x >= world->getWidth() ||
                        alt.y < 0 || alt.y >= world->getHeight()) {
                        continue;
                    }

                    // Sprawdź przejezdność
                    Tile* altTile = world->getTile(alt);
                    if (!altTile || !altTile->isTraversable()) {
                        continue;
                    }

                    // Sprawdź czy wolna
                    if (world->getOrganismAt(alt) == nullptr) {
                        alternatives.push_back(alt);
                    }
                }
            }


            if (!alternatives.empty()) {
                // Wybierz losową wolną pozycję
                int randomIndex = rand() % alternatives.size();
                plannedPosition = alternatives[randomIndex];

            } else {

                plannedPosition = position;
                isMoving = false;
                return;
            }
        }
        lastPosition = position;
        position = plannedPosition;
        isMoving = true;
        actionPoints--;

        // Koszt ruchu
        setEnergy(energy - 1.0);

        //  efekty tile po wejściu
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

