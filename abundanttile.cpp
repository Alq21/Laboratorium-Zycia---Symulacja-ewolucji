#include "abundanttile.h"
#include "organism.h"

AbundantTile::AbundantTile(Position pos) : Tile(pos, Color(100, 210, 100)) {}

void AbundantTile::applyEffect(Organism* organism) {
}

double AbundantTile::getProductionBonus() const {
    return 1.5; // np   50% więcej energii z fotosyntezy
}