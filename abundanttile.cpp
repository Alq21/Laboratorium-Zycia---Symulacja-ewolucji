#include "abundanttile.h"
#include "organism.h"
#include "producer.h"

namespace {
constexpr double BasePhotosynthesisGain = 6.2;
}

AbundantTile::AbundantTile(Position pos) : Tile(pos, Color(100, 210, 100)) {}

void AbundantTile::applyEffect(Organism* organism) {
    if (!organism || !organism->getIsAlive()) {
        return;
    }

    auto* producer = dynamic_cast<Producer*>(organism);
    if (!producer) {
        return;
    }

    const double bonusGain = BasePhotosynthesisGain * (getProductionBonus() - 1.0);
    producer->photosynthesize(bonusGain);
}

double AbundantTile::getProductionBonus() const {
    return 1.5;
}
