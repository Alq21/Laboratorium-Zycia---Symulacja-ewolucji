#include "Organism.h"
#include "World.h"
#include <stdlib.h>
#include "tile.h"

Organism::Organism(Position pos, Color col, double startEnergy, double maxEn, int size, int speed, int maxAP, int gen)
    : Entity(pos, col), energy(startEnergy), maxEnergy(maxEn), isAlive(true), size(size), speed(speed),
    actionPoints(maxAP), maxActionPoints(maxAP), plannedPosition(pos), generation(gen) {
}

void Organism::setEnergy(int newEnergy) {
    energy = newEnergy;
    if (energy <= 0) {
        energy = 0;
        die();
    }
}

void Organism::die() {
    isAlive = false;
    energy=0;

}

void Organism::onTick(World* world) {
    if (!isAlive) return;

    energy -= 10; // Co turę organizm traci np. trochę energii. Jeśli spadnie do 0, umiera
    if (energy <= 0) {
        die();
    }

    // Odnawiamy punkty akcji na nową turę
    actionPoints = maxActionPoints;
}



void Organism::planMove(World* world) {
    if (!isAlive || actionPoints <= 0) return;

    // Najprostsza implementacja: zaplanuj ruch w losowym kierunku
    // W drapieżniku (Predator) NADPISZESZ tę funkcję, by szukała jedzenia!

    int newX = position.x + (rand() % 3 - 1);
    int newY = position.y + (rand() % 3 - 1);

    plannedPosition = Position(newX, newY);}

void Organism::onInteract(Entity* other) {

}


bool Organism::canReproduce() const {
    // Przykładowy warunek: może się rozmnożyć, jeśli ma dużo energii i żyje
    return isAlive && energy > 50;
}

void Organism::executeMovement(World* world) {
    if (!isAlive || actionPoints <= 0) {return;}
Tile* targetTile = world->getTile(plannedPosition);
    if (targetTile == nullptr) {
        plannedPosition = position; // Anulujemy plan (zostajemy w miejscu)
        return;
    }

    if (position.x != plannedPosition.x || position.y != plannedPosition.y) {
        position = plannedPosition;
        actionPoints--;
    }


}
int Organism::getGeneration() const {
    return generation;
}
void Organism::setEnergy(double newEnergy) {
    energy = newEnergy;
    if (energy <= 0.0) {
        die();
    }
}
bool Organism::getIsAlive() const {
    return isAlive;
}

double Organism::getEnergy() const {
    return energy;
}