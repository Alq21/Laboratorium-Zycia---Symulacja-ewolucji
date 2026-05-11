#ifndef ENTITY_H
#define ENTITY_H

#include "position.h"
#include <QColor>

class Entity {
protected:
    Position position;
    QColor color;

public:
    Entity(Position pos, QColor col) ;
    virtual ~Entity() = default;

    Position getPosition() const;
    QColor getColor() const ;
};

#endif // ENTITY_H