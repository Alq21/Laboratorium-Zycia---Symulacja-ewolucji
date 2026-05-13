#include "impassabletile.h"

ImpassableTile::ImpassableTile(Position pos) : Tile(pos, Color(60, 60, 60)) {}

bool ImpassableTile::blockMovement() const {
    return true;
}

void ImpassableTile::applyEffect(Organism* organism) {
}