#include "abundanttile.h"
#include "organism.h"

AbundantTile::AbundantTile(Position pos) : Tile(pos, QColor(0, 255, 100)) {}

void AbundantTile::applyEffect(Organism* organism) {
}

double AbundantTile::getProductionBonus() const {
    return 1.5; // np   50% więcej energii z fotosyntezy
}