#ifndef PRODUCER_H
#define PRODUCER_H

#include "organism.h"

class Tile;

class Producer : public Organism {
public:

   Producer(Position pos, Color col, double startEnergy, double maxEn, int size, int speed, int maxAP, int gen,
            TemperaturePreference tempPreference = TemperaturePreference::Default);

    virtual ~Producer() = default;

    void onTick(World* world) override;
    void planMove(World* world) override;
    void onInteract(Entity* other) override;

    bool canReproduce() const override;
    std::unique_ptr<Organism> reproduce() override;

    void photosynthesize(double amount);

    static int awarenessRadius();
};

#endif // PRODUCER_H
