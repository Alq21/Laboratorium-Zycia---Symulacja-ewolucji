#include "entity.h"

Entity::Entity(Position pos, QColor col) : position(pos), color(col) {
}

Position Entity::getPosition() const  {
    return position;
}

QColor Entity::getColor() const {
    return color;
}