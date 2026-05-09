#include "organism.h"


Organism::Organism(Position pos, Color col, double energy, int size, int speed, int maxAP)
    : Entity(pos, col), _energy(energy), _isAlive(true), _size(size),
    _speed(speed), _actionPoints(maxAP), _maxActionPoints(maxAP) {
}

// Co turę organizm traci np. trochę energii. Jeśli spadnie do 0, umiera.
void Organism::onTick(World* world) {
    _energy -= 1.0; // Przykładowy spadek energii
    if (_energy <= 0.0) {
        _isAlive = false;
    }
    // Odnawiamy punkty akcji na nową turę
    _actionPoints = _maxActionPoints;
}

void Organism::onInteract(Entity* other) {

}

bool Organism::canReproduce() const {
    // Przykładowy warunek: może się rozmnożyć, jeśli ma dużo energii i żyje
    return _isAlive && _energy > 50.0;
}

void Organism::executeMovement(World* world) {

}

bool Organism::isAlive() const {
    return _isAlive;
}

double Organism::getEnergy() const {
    return _energy;
}