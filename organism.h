#ifndef ORGANISM_H
#define ORGANISM_H
#include "entity.h"
#include <_mingw_mac.h>
class World;
class Organism: public Entity
{
protected:
    double _energy;
    bool _isAlive;
    int _size;
    int _speed;
    int _actionPoints;
    const int _maxActionPoints;

public:
   Organism(Position pos, Color col, double energy, int size, int speed, int maxAP);
virtual ~Organism() = default;

   // void getColor();
   //  void getPosition();


    virtual void onTick(World* world);
    virtual void onInteract(Entity* other);
    bool canReproduce() const;
    void executeMovement(World* world);
    virtual Organism* reproduce() = 0;
    virtual Position planMove(World* world) = 0;


    bool isAlive() const;
    double getEnergy() const;

};

#endif // ORGANISM_H
