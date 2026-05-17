#ifndef PREDATOR_H
#define PREDATOR_H

#include "organism.h"

class Predator : public Organism {
private:
    int visionRange;
    Position findNearestPrey(World* world);

public:
    Predator(Position pos, Color col, double startEnergy, double maxEn, int size, int speed, int maxAP, int gen, int vision);

void onTick(World* world) override;
    void planMove(World* world) override;
    void onInteract(Entity* other) override;
    int getVision(){return visionRange;}
    bool canReproduce() const override;
    std::unique_ptr<Organism> reproduce() override;

    void hunt(Organism* target);
};

#endif // PREDATOR_H