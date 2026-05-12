#ifndef ABUNDANTTILE_H
#define ABUNDANTTILE_H
#include "tile.h"

class AbundantTile : public Tile {
public:
    AbundantTile(Position pos);
    //Zielony kolor

    void applyEffect(Organism* organism) override;
    double getProductionBonus() const;
};

#endif // ABUNDANTTILE_H
