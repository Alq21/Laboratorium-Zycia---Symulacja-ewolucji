#include "entity.h"

Entity::Entity(Position pos, Color col) : position(pos), color(col) {
}

Position Entity::getPosition() const  {
    return position;
}

void Entity::setPosition(Position pos) {
    position = pos;
}

Color Entity::getColor() const {
    return color;
}