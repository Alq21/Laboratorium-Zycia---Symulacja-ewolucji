#ifndef POSITION_H
#define POSITION_H

struct Position {
    int x;
    int y;

    Position(int _x = 0, int _y = 0) : x(_x), y(_y) {}
    bool operator==(const Position& other) const {
        return x == other.x && y == other.y;
    }
};

#endif // POSITION_H
