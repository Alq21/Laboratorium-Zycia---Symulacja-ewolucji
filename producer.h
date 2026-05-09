#ifndef PRODUCER_H
#define PRODUCER_H

#include "organism.h"


class Tile;

class Producer : public Organism {
protected:
    double _preferredTemperature;

public:

    Producer(Position pos, Color col, double energy, int size, int speed, int maxAP, double prefTemp);

    virtual ~Producer() = default;


    void photosynthesis(Tile* currentTile);


};

#endif // PRODUCER_H
