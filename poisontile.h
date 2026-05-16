#ifndef POISONTILE_H
#define POISONTILE_H
#include "tile.h"

class PoisonTile : public Tile {
public:
    PoisonTile(Position pos);

    void applyEffect(Organism* organism) override;
};

#endif // POISONTILE_H
