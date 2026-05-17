#ifndef BOARDSNAPSHOT_H
#define BOARDSNAPSHOT_H

#include <vector>
#include "entity.h"
#include "position.h"
#include "temperaturepreference.h"

enum class BoardTileKind {
    Normal,
    Abundant,
    Poison,
    Impassable
};

struct BoardTileSnapshot {
    Position      position;
    BoardTileKind kind        = BoardTileKind::Normal;
    double        temperature = 20.0;
    double        humidity    = 50.0;
    double        sunlight    = 50.0;
};

struct BoardOrganismSnapshot {
    Position              position;
    Position              lastPosition;
    Color                 color;
    int                   size        = 1;
    bool                  predator    = false;
    bool                  omnivore    = false;
    TemperaturePreference temperaturePreference = TemperaturePreference::Default;
};

struct BoardSnapshot {
    int width  = 0;
    int height = 0;
    std::vector<BoardTileSnapshot>    tiles;
    std::vector<BoardOrganismSnapshot> organisms;
    std::vector<Position>             deathPositions;  // ← pozycje śmierci w tym ticku

    bool isEmpty() const {
        return tiles.empty() && organisms.empty();
    }
};

#endif // BOARDSNAPSHOT_H
