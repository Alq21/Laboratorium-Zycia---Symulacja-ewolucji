#ifndef ENTITY_H
#define ENTITY_H

#include "position.h"
#include <QColor>

class Entity {
protected:
    Position position;
    QColor color;

public:
    Entity(Position pos, QColor col) : position(pos), color(col) {}
    virtual ~Entity() = default;

    Position getPosition() const { return position; }
    QColor getColor() const { return color; }
};

#endif // ENTITY_H