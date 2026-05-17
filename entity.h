#ifndef ENTITY_H
#define ENTITY_H

#include "position.h"


struct Color {
    int r, g, b;
    Color(int r = 255, int g = 255, int b = 255) : r(r), g(g), b(b) {}
};

class Entity {
protected:
    Position position;
    Color color;

public:
    Entity(Position pos, Color col) ;
    virtual ~Entity() = default;

    Position getPosition() const;
    void setPosition(Position pos);
    Color getColor() const ;
};

#endif // ENTITY_H