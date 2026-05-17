#ifndef OMNIVORE_H
#define OMNIVORE_H

#include "organism.h"

class Omnivore : public Organism {
private:
    int visionRange;
    Organism* findNearestPrey(World* world) const;

public:
    Omnivore(Position pos, Color col, double startEnergy, double maxEn, int size, int speed, int maxAP, int gen,
             int vision, TemperaturePreference tempPreference = TemperaturePreference::Default);

    void onTick(World* world) override;
    void planMove(World* world) override;
    void onInteract(Entity* other) override;
    int getVision() const;
    bool canReproduce() const override;
    std::unique_ptr<Organism> reproduce() override;

    bool attemptHunt(Organism* target);
};

#endif // OMNIVORE_H
