#include "predator.h"

Predator::Predator(Position pos, Color col, double energy, int size, int speed, int maxAP)
    : Organism(pos, col, energy, size, speed, maxAP) {
}

void Predator::hunt(Organism* target) {
    // Jeśli ofiara żyje, drapieżnik zyskuje jej energię )
    if (target && target->isAlive()) {
        this->_energy += target->getEnergy();
        // Można tu później wywołać jakąś metodę np. target->die();
    }
}

void Predator::onTick(World* world) {

    Organism::onTick(world);


    this->_energy -= 2.0;
}

Organism* Predator::reproduce() {
    // Prosta mutacja: Tworzymy nowego Predatora obok starego z drobnymi zmianami!
    Position childPos = { _position.x + 1, _position.y + 1 };
    Color childCol = { _color.r, _color.g - 10, _color.b }; // Delikatna zmiana koloru (ewolucja)

    return new Predator(childPos, childCol, 100.0, _size + 1, _speed, _maxActionPoints);
}

Position Predator::planMove(World* world) {
    // Na razie, bez klasy World, drapieżnik idzie po prostu w prawo i w dół (dummy logic)
    return Position{_position.x + 1, _position.y + 1};
}
