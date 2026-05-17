#include "poisontile.h"
#include "organism.h"

PoisonTile::PoisonTile(Position pos) : Tile(pos, Color(210, 100, 100)) {}

void PoisonTile::applyEffect(Organism* organism) {
    if (organism && organism->getIsAlive()) {
        double current = organism->getEnergy();
        organism->setEnergy(current - 14.0);
    }
}