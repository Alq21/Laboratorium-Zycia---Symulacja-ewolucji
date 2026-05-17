#ifndef PREDATOR_H
#define PREDATOR_H

#include "organism.h"

class Producer;

class Predator : public Organism {
private:
    int visionRange;
    bool leapPlanned  = false;
    bool isLeapingNow = false;
    int  leapCooldown = 0;

    // Szuka najlepszej ofiary typu Producer
    Producer* findBestPreyProducer(World* world) const;

    bool canLeap() const;

public:
    Predator(Position pos, Color col, double startEnergy, double maxEn, int size, int speed, int maxAP, int gen,
             int vision, TemperaturePreference tempPreference = TemperaturePreference::Default);

    void onTick(World* world) override;
    void planMove(World* world) override;
    void executeMovement(World* world) override;
    void onInteract(Entity* other) override;

    int  getVision()     const;
    int  getBaseVision() const { return visionRange; }
    bool getIsLeaping()  const { return isLeapingNow; }

    bool canReproduce() const override;
    std::unique_ptr<Organism> reproduce() override;

    bool attemptHunt(Organism* target);
};

#endif // PREDATOR_H