#include "world.h"
#include "organism.h"
#include "normaltile.h"
#include "abundanttile.h"
#include "poisontile.h"
#include "impassabletile.h"
#include "tile.h"
#include <cstdlib>
#include <ctime>
#include <algorithm>


World::World(int w, int h, MapConfig config) : width(w), height(h) {
    std::srand(std::time(nullptr));
    habitat.resize(height);
    generateMap(config);
}

void World::generateMap(const MapConfig& config) {
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            Position pos(x, y);

            // Ustawienie wag bazowych z konfiguracji
            int weightNormal = config.baseNormalWeight;
            int weightAbundant = config.baseAbundantWeight;
            int weightPoison = config.basePoisonWeight;
            int weightImpassable = config.baseImpassableWeight;

            // Modyfikacja wag na podstawie sąsiadów (tworzenie biomów)
            // Sprawdzanie sąsiada z lewej
            if (x > 0) {
                Tile* leftTile = habitat[y][x - 1].get();
                if (dynamic_cast<AbundantTile*>(leftTile)) weightAbundant += config.clusterBonus;
                else if (dynamic_cast<PoisonTile*>(leftTile)) weightPoison += config.clusterBonus;
                else if (dynamic_cast<ImpassableTile*>(leftTile)) weightImpassable += config.clusterBonus;
            }

            // Sprawdzanie sąsiada z góry
            if (y > 0) {
                Tile* topTile = habitat[y - 1][x].get();
                if (dynamic_cast<AbundantTile*>(topTile)) weightAbundant += config.clusterBonus;
                else if (dynamic_cast<PoisonTile*>(topTile)) weightPoison += config.clusterBonus;
                else if (dynamic_cast<ImpassableTile*>(topTile)) weightImpassable += config.clusterBonus;
            }

            // Losowanie ruletkowe
            int totalWeight = weightNormal + weightAbundant + weightPoison + weightImpassable;
            int randomValue = std::rand() % totalWeight;

            // Przypisanie odpowiedniej płytki na podstawie wylosowanej wartości
            if (randomValue < weightNormal) {
                habitat[y].push_back(std::make_unique<NormalTile>(pos));
            }
            else if (randomValue < weightNormal + weightAbundant) {
                habitat[y].push_back(std::make_unique<AbundantTile>(pos));
            }
            else if (randomValue < weightNormal + weightAbundant + weightPoison) {
                habitat[y].push_back(std::make_unique<PoisonTile>(pos));
            }
            else {
                habitat[y].push_back(std::make_unique<ImpassableTile>(pos));
            }
        }
    }
}

Tile* World::getTile(Position p) const {
        // Sprawdzanie czy współrzędne nie są poza mapą
    if (p.x >= 0 && p.x < width && p.y >= 0 && p.y < height) {
        return habitat[p.y][p.x].get();
    }
    return nullptr;
}

void World::setTile(Position p, std::unique_ptr<Tile> newTile) {
    if (p.x >= 0 && p.x < width && p.y >= 0 && p.y < height) {
        habitat[p.y][p.x] = std::move(newTile);
    }
}

void World::addOrganism(std::unique_ptr<Organism> o) {
    if (o) {
        organisms.push_back(std::move(o));
    }
}

Organism* World::getOrganismAt(Position p) const {
    for (const auto& org : organisms) {
        if (org->getPosition() == p && org->getIsAlive()) {
            return org.get();
        }
    }
    return nullptr;
}

void World::removeDead() {
    organisms.erase(
        std::remove_if(organisms.begin(), organisms.end(),
        [](const std::unique_ptr<Organism>& o) {
        return !o->getIsAlive();
        }),
        organisms.end()
        );
}
World::~World() = default;