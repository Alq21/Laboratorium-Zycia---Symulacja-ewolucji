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
};

class World {
private:
    int width;
    int height;
    std::vector<std::vector<std::unique_ptr<Tile>>> habitat;
    std::vector<std::unique_ptr<Organism>> organisms;
    void generateMap(const MapConfig& config);
    EnvironmentParameters globalParameters;
public:
    World(int w, int h, MapConfig config = MapConfig());
    ~World() ;

    // Metody zarządzania płytkami:
    Tile* getTile(Position p) const;
    void setTile(Position p, std::unique_ptr<Tile> newTile);
    void addOrganism(std::unique_ptr<Organism> o);
    void removeDead();
    Organism* getOrganismAt(Position p) const;
    //metoda do zaludnienia świata, potem chcielismy żeby to czytało z pliku wiec trzeba bedzie  udoskonalić
    void populate(int numProducers, int numPredators);


    int getWidth() const { return width; }
    int getHeight() const { return height; }

    const std::vector<std::unique_ptr<Organism>>& getOrganisms() const { return organisms; }
    World(const World&) = delete;
    World& operator=(const World&) = delete;
    // dla parametrów srodowiska
    void setGlobalParameters(EnvironmentParameters parameters);
    EnvironmentParameters getCombinedParameters(Position pos) const;
    
    // Liczenie populacji danego typu
    template<typename T>
    int countPopulation() const {
        int count = 0;
        for (const auto& org : organisms) {
            if (org->getIsAlive() && dynamic_cast<T*>(org.get())) {
                count++;
            }
        }
        return count;
    }
};

#endif // WORLD_H
