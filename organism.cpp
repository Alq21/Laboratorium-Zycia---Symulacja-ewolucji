#include "Organism.h"
#include "World.h"
#include "entity.h"
#include "tile.h"

Organism::Organism(Position pos, QColor col, int energy, int size, int speed, int maxAP)
    : Entity(pos, col),
    energy(energy),
    isAlive(true),
    size(size),
    speed(speed),
    actionPoints(maxAP),
    maxActionPoints(maxAP),
    plannedPosition(pos)
{}

void Organism::setEnergy(int newEnergy) {
    energy = newEnergy;
    if (energy <= 0) {
        energy = 0;
        die();
    }
}

void Organism::die() {
    isAlive = false;
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

void Organism::onInteract(Entity* other) {
    // Domyślnie brak interakcji, nadpiszą to Predator/Producer
}

//Fajnie byłoby uzależnić koszt rozmnażania się od cech bakterii (np. większa=więcej energii)
bool Organism::canReproduce() const {
    // Przykładowy warunek: może się rozmnożyć, jeśli ma dużo energii i żyje
    return isAlive && energy > 50;
}

void Organism::executeMovement(World* world) {
    if (!isAlive || actionPoints <= 0) return;

    if (position.x != plannedPosition.x || position.y != plannedPosition.y) {
        // Tutaj mogłaby wejść logika kosztu PA w zależności od dystansu
        // Na razie proste przemieszczenie:
        position = plannedPosition;
        Tile* currentTile = world->getTile(position);
        if (currentTile) {
            currentTile->applyEffect(this);
        }
    }
}

bool Organism::getIsAlive() const {
    return isAlive;
}

double Organism::getEnergy() const {
    return energy;
}