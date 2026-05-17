#include "tile.h"

Tile::Tile(Position pos, Color col) : Entity(pos, col) {}
EnvironmentParameters& Tile::getLocalModifiers() {
    return localModifiers;
}