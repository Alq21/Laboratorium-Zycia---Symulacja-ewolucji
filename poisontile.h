#ifndef POISONTILE_H
#define POISONTILE_H
#include "tile.h"

class PoisonTile : public Tile {
public:
    PoisonTile(Position pos) : Tile(pos, QColor(200, 0, 0)) {} // Czerwony kolor

    void applyEffect(Organism* organism) override;
};

#endif // POISONTILE_H
