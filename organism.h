#ifndef ORGANISM_H
#define ORGANISM_H

#include "entity.h"
#include "temperaturepreference.h"
#include <memory>

class World;

class Organism : public Entity {
public:
    Organism(Position pos, Color col, double startEnergy, double maxEn, int size, int speed, int maxAP, int gen,
             TemperaturePreference tempPreference = TemperaturePreference::Default);
    virtual ~Organism() = default;

    virtual void onTick(World* world);
    virtual void planMove(World* world);
    virtual void executeMovement(World* world);
    virtual void onInteract(Entity* other);
    virtual bool canReproduce() const;
    virtual Position findPlaceToBreed(World* world) const;
    virtual std::unique_ptr<Organism> reproduce() = 0;

    void setEnergy(double newEnergy);
    void die();

    bool getIsAlive() const;
    double getEnergy() const;
    int getGeneration() const;
    int getSize() const;
    Color getColor() const;
    int getSpeed() const;
    int getActionPoints() const { return actionPoints; }
    Position getLastPosition() const { return lastPosition; }
    TemperaturePreference getTemperaturePreference() const;
    int getLineageId() const { return lineageId; }
    int getParentLineageId() const { return parentLineageId; }
    bool isDirectKin(const Organism* other) const;
    void initLineageFromParent(const Organism* parent);

    int movementApCost() const;
    int reproductionApCost() const;
    double reproductionEnergyCost() const;
    bool trySpendActionPoints(int apCost);
    void refillActionPoints();

    double climateMetabolismFactor(World* world) const;

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
    TemperaturePreference temperaturePreference;
    int lineageId;
    int parentLineageId;
};

#endif // ORGANISM_H
