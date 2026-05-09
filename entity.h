#ifndef ENTITY_H
#define ENTITY_H


struct Position
{   int x;
    int y;

};

struct Color {
    int r, g, b;


    Color(int r = 255, int g = 255, int b = 255) : r(r), g(g), b(b) {}
};

class Entity

{
protected:
    Position _position;
    Color _color;
public:
    Entity(Position pos, Color col);

    Position getPosition();
   Color getColor();
};




#endif // ENTITY_H