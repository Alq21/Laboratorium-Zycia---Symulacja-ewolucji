#ifndef ORGANISM_H
#define ORGANISM_H

#include "entity.h"
#include <memory>


class World;

class Organism : public Entity {
protected:
    int energy;
    double maxEnergy;
    bool isAlive;
    int size;
    int speed;
    int actionPoints;
    int maxActionPoints;
    Position plannedPosition;
    int generation;

public:
   Organism(Position pos, Color col, double startEnergy, double maxEn, int size, int speed, int maxAP, int gen);
    virtual ~Organism() = default;

    virtual void onTick(World* world) =0;


    virtual void planMove(World* world);
    virtual void executeMovement(World* world);
    void setEnergy(int newEnergy);
    void die();

    virtual void onInteract(Entity* other);
    virtual bool canReproduce() const;
    virtual std::unique_ptr<Organism> reproduce() = 0;

    // Gettery:
    bool getIsAlive() const;
    double getEnergy() const;
    void setEnergy(double newEnergy);
    int getGeneration() const;
    int getSize() const;
    int getSpeed() const;
};

#endif // ORGANISM_HSSS