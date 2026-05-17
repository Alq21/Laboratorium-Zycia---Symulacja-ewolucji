#ifndef ORGANISM_H
#define ORGANISM_H

#include "entity.h"
#include <memory>

class World;

class Organism : public Entity {
public:
    Organism(Position pos, Color col, double startEnergy, double maxEn, int size, int speed, int maxAP, int gen);
    virtual ~Organism() = default;

    // TYLKO DEKLARACJE - BEZ {}!
    virtual void onTick(World* world);
    virtual void planMove(World* world);
    virtual void executeMovement(World* world);
    virtual void onInteract(Entity* other);
    virtual bool canReproduce() const;
    virtual std::unique_ptr<Organism> reproduce() = 0;

    void setEnergy(double newEnergy);
    void die();

    bool getIsAlive() const;
    double getEnergy() const;
    int getGeneration() const;
    int getSize() const;
    Color getColor() const;
    int getSpeed() const;
    Position getLastPosition() const { return lastPosition; }

protected:
    double energy;
    double maxEnergy;
    bool isAlive;
    int size;
    int speed;
    int actionPoints;
    int maxActionPoints;
    Position plannedPosition;
    int generation;
    Position lastPosition;
    bool isMoving;
};

#endif // ORGANISM_H


