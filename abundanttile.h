#ifndef ABUNDANTTILE_H
#define ABUNDANTTILE_H
#include "tile.h"

class AbundantTile : public Tile {
public:
    AbundantTile(Position pos) : Tile(pos, QColor(0, 200, 0)) {} //Zielony kolor

    void applyEffect(Organism* organism) override;
};

#endif // ABUNDANTTILE_H
