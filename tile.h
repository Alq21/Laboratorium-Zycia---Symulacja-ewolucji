#ifndef TILE_H
#define TILE_H

#include "entity.h"
#include "environmentparameters.h"

class Organism;

class Tile : public Entity {

    EnvironmentParameters localModifiers;
public:
    Tile(Position pos, Color col);
    virtual ~Tile() = default;
    virtual void applyEffect(Organism* organism) = 0;
    virtual bool isTraversable() const { return true; }
    EnvironmentParameters& getLocalModifiers();
};

#endif // TILE_H