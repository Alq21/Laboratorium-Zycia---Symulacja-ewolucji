#ifndef ORGANISM_H
#define ORGANISM_H

#include "entity.h"
#include <QColor>
#include <memory>

class World;

class Organism : public Entity {
protected:
    int energy;
    bool isAlive;
    int size;
    int speed;
    int actionPoints;
    int maxActionPoints;
    Position plannedPosition;

public:
    Organism(Position pos, QColor col, int energy, int size, int speed, int maxAP);
    virtual ~Organism() = default;

    virtual void onTick(World* world);
    virtual void planMove(World* world) = 0;
    virtual void executeMovement(World* world);
    void setEnergy(int newEnergy);
    void die();

    virtual void onInteract(Entity* other);
    virtual bool canReproduce() const;
    virtual std::unique_ptr<Organism> reproduce() = 0;

    // Gettery:
    bool getIsAlive() const;
    double getEnergy() const;
};

#endif // ORGANISM_H