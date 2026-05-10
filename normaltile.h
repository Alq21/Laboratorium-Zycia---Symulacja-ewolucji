#ifndef NORMALTILE_H
#define NORMALTILE_H

#include "tile.h"

class NormalTile : public Tile {
public:
    NormalTile(Position pos);
    void applyEffect(Organism* organism) override;
};

#endif