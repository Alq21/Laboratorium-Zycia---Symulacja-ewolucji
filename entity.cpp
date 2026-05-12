#include "entity.h"

Entity::Entity(Position pos, QColor col) : position(pos), color(col) {
}

Position Entity::getPosition()  {
    return position;
}

QColor Entity::getColor() {
    return color;
}