#include "abundanttile.h"
#include "organism.h"

AbundantTile::AbundantTile(Position pos) : Tile(pos, Color(100, 210, 100)) {}

void AbundantTile::applyEffect(Organism* organism) {
    if (organism && organism->getIsAlive()) {
        double current = organism->getEnergy();
        organism->setEnergy(current + 10);
    }
}

double AbundantTile::getProductionBonus() const {
    return 1.5;
}
