#include "entity.h"

Entity::Entity(Position pos, Color col) : _position(pos), _color(col) {
}

Position Entity::getPosition()  {
    return _position;
}

Color Entity::getColor() {
    return _color;
}