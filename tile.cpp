#include "tile.h"

Tile::Tile(Position pos, Color col) : Entity(pos, col), localModifiers(0.0, 0.0, 0.0) {}
EnvironmentParameters& Tile::getLocalModifiers() {
    return localModifiers;
}
