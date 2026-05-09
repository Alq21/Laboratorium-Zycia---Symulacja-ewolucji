#ifndef WORLD_H
#define WORLD_H
#include "organism.h"
#include <list>
// jak coś dodałam tą klase bym mogła zaczynać robić swoją część
class World
{
    int widght;
    int height;
std::list<Organism*> organisms;


public:
    World();
    void removeDead();
    void addOrganism(Organism o);
};

#endif // WORLD_H
