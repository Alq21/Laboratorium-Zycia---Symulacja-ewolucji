#include "impassabletile.h"

ImpassableTile::ImpassableTile(Position pos) : Tile(pos, Color(50, 50, 50)) {} // Szary

bool ImpassableTile::blockMovement() const {
    return true;
}

void ImpassableTile::applyEffect(Organism* organism) {
}