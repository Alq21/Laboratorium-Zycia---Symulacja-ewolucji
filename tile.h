#ifndef TILE_H
#define TILE_H

#include "entity.h"

class Organism;

class Tile : public Entity {
public:
    Tile(Position pos, QColor col);
    virtual ~Tile() = default;
    virtual void applyEffect(Organism* organism) = 0;
    virtual bool isTraversable() const { return true; }
};

#endif // TILE_H