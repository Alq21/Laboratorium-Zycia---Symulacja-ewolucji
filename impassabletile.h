#ifndef IMPASSABLETILE_H
#define IMPASSABLETILE_H
#include "tile.h"

class ImpassableTile : public Tile {
public:
    ImpassableTile(Position pos);
    bool blockMovement() const;
    void applyEffect(Organism* organism) override;
    bool isTraversable() const override;
};

#endif // IMPASSABLETILE_H
