#include "poisontile.h"
#include "organism.h"

PoisonTile::PoisonTile(Position pos) : Tile(pos, QColor(200, 0, 0)) {} // Czerwony

void PoisonTile::applyEffect(Organism* organism) {
    if (organism && organism->getIsAlive()) {
        double current = organism->getEnergy();
        organism->setEnergy(current - 20); //np 20
    }
}