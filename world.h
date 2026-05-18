#ifndef WORLD_H
#define WORLD_H

#include <vector>
#include <memory>
#include "environmentparameters.h"
#include "position.h"
#include "organism.h"

class Tile;
class Organism;

struct MapConfig {
    int baseNormalWeight = 100;
    int baseAbundantWeight = 8;
    int basePoisonWeight = 4;
    int baseImpassableWeight = 5;
    int clusterBonus = 150;
    int baseTemperateWeight = 100;
    int baseColdWeight = 10;
    int baseWarmWeight = 10;
    int temperatureClusterBonus = 120;
};

class World {
private:
    int width;
    int height;
    std::vector<std::vector<std::unique_ptr<Tile>>> habitat;
    std::vector<std::unique_ptr<Organism>> organisms;
    void generateMap(const MapConfig& config);
    EnvironmentParameters globalParameters;
    EnvironmentParameters eventModifiers;  // DODANE: zmiany z wydarzeń klimatycznych

    // Pozycje organizmow, ktore zginely w tym ticku
    std::vector<Position> recentDeaths_;

public:
    World(int w, int h, MapConfig config = MapConfig());
    ~World();

    // Metody zarządzania płytkami:
    Tile* getTile(Position p) const;
    void setTile(Position p, std::unique_ptr<Tile> newTile);
    void addOrganism(std::unique_ptr<Organism> o);
    void removeDead();          // zbiera recentDeaths_ przed usunięciem
    Organism* getOrganismAt(Position p) const;
    void populate(int numProducers, int numPredators);

    Position findFreeTraversablePosition() const;

    int getWidth()  const { return width;  }
    int getHeight() const { return height; }

    const std::vector<std::unique_ptr<Organism>>& getOrganisms() const { return organisms; }

    // Ostatnie śmierci — odczyt przez collectSnapshot(), potem clear
    const std::vector<Position>& getRecentDeaths() const { return recentDeaths_; }
    void clearRecentDeaths() { recentDeaths_.clear(); }

    World(const World&) = delete;
    World& operator=(const World&) = delete;

    void setGlobalParameters(EnvironmentParameters parameters);
    void setEventModifiers(EnvironmentParameters modifiers);  // DODANE: ustaw zmiany z wydarzeń
    EnvironmentParameters getCombinedParameters(Position pos) const;

    template<typename T>
    int countPopulation() const {
        int count = 0;
        for (const auto& org : organisms)
            if (org->getIsAlive() && dynamic_cast<T*>(org.get()))
                ++count;
        return count;
    }
};

#endif // WORLD_H
