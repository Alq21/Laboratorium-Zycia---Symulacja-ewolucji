#include "entity.h"

Entity::Entity(Position pos, Color col) : position(pos), color(col) {
}

Position Entity::getPosition() const  {
    return position;
}

Color Entity::getColor() const {
    return color;
}