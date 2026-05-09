#ifndef PREDATOR_H
#define PREDATOR_H

#include "organism.h"

class Predator : public Organism {
public:
    Predator(Position pos, Color col, double energy, int size, int speed, int maxAP);
    ~Predator() override = default;


    void hunt(Organism* target);


    void onTick(World* world) override;
    Organism* reproduce() override;
    Position planMove(World* world) override;
};

#endif // PREDATOR_H