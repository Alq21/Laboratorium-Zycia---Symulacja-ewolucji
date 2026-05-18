#include "world.h"
#include "organism.h"
#include "normaltile.h"
#include "abundanttile.h"
#include "poisontile.h"
#include "impassabletile.h"
#include "predator.h"
#include "producer.h"
#include "movementplanning.h"
#include "tile.h"
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <qdebug.h>
#include <qlogging.h>

namespace {
constexpr double ColdTemperatureModifier = -25.0;
constexpr double WarmTemperatureModifier = 25.0;
constexpr double ColdNeighborThreshold = -10.0;
constexpr double WarmNeighborThreshold = 10.0;

void applyTemperatureBiome(Tile* tile, double temperatureModifier) {
    if (!tile) return;
    tile->getLocalModifiers().modifyParameter("temperature", temperatureModifier);
}
}

World::World(int w, int h, MapConfig config) 
    : width(w), 
      height(h),
      globalParameters(20.0, 50.0, 50.0),  // DODANE: domyślne parametry
      eventModifiers(0.0, 0.0, 0.0)        // DODANE: brak wydarzeń na start
{
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

            int weightTemperate = config.baseTemperateWeight;
            int weightCold = config.baseColdWeight;
            int weightWarm = config.baseWarmWeight;

            if (x > 0) {
                double leftTemp = habitat[y][x - 1]->getLocalModifiers().getTemperature();
                if (leftTemp <= ColdNeighborThreshold) weightCold += config.temperatureClusterBonus;
                else if (leftTemp >= WarmNeighborThreshold) weightWarm += config.temperatureClusterBonus;
            }

            if (y > 0) {
                double topTemp = habitat[y - 1][x]->getLocalModifiers().getTemperature();
                if (topTemp <= ColdNeighborThreshold) weightCold += config.temperatureClusterBonus;
                else if (topTemp >= WarmNeighborThreshold) weightWarm += config.temperatureClusterBonus;
            }

            int totalTemperatureWeight = weightTemperate + weightCold + weightWarm;
            int temperatureRoll = std::rand() % totalTemperatureWeight;
            double temperatureModifier = 0.0;

            if (temperatureRoll < weightCold) {
                temperatureModifier = ColdTemperatureModifier;
            }
            else if (temperatureRoll < weightCold + weightWarm) {
                temperatureModifier = WarmTemperatureModifier;
            }

            applyTemperatureBiome(habitat[y].back().get(), temperatureModifier);
        }
    }
}
Position World::findFreeTraversablePosition() const {
    // Próbuj losowe pozycje, maks. 500 prób, żeby uniknąć nieskończonej pętli
    for (int attempt = 0; attempt < 500; ++attempt) {
        int rx = std::rand() % width;
        int ry = std::rand() % height;
        Position pos{rx, ry};
        Tile* tile = getTile(pos);
        if (tile && tile->isTraversable() && getOrganismAt(pos) == nullptr) {
            return pos;
        }
    }
    // Jeśli nie znaleziono losowo, szukaj liniowo
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            Position pos{x, y};
            Tile* tile = getTile(pos);
            if (tile && tile->isTraversable() && getOrganismAt(pos) == nullptr) {
                return pos;
            }
        }
    }
    return Position{-1, -1}; // Brak wolnego miejsca
}

void World::populate(int numProducers, int numPredators) {
    // Zaludniamy świat Producentami
    for (int i = 0; i < numProducers; ++i) {
        Position pos = findFreeTraversablePosition();
        if (pos.x < 0) { qDebug() << "populate: brak wolnego miejsca dla Producenta"; break; }

        auto producer = std::make_unique<Producer>(
            pos, Color{0, 255, 0}, 50.0, 100.0, 1, 1, 1, 1
            );
        addOrganism(std::move(producer));
    }

    // Zaludniamy świat Drapieżnikami
    for (int i = 0; i < numPredators; ++i) {
        Position pos = findFreeTraversablePosition();
        if (pos.x < 0) { qDebug() << "populate: brak wolnego miejsca dla Drapieżnika"; break; }

        auto predator = std::make_unique<Predator>(
            pos, Color{255, 0, 0}, 100.0, 200.0, 1, 1, 1, 1,
            MovementPlanning::minimumPredatorVisionRadius()
            );
        addOrganism(std::move(predator));
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



    // Funkcja addOrganism - CAŁA NOWA WERSJA:
    void World::addOrganism(std::unique_ptr<Organism> o) {
        if (!o) {
            qDebug() << "  ✗ Cannot add null organism";
            return;
        }

        Position pos = o->getPosition();

        // Walidacja granic mapy
        if (pos.x < 0 || pos.x >= width || pos.y < 0 || pos.y >= height) {
            qDebug() << "  ✗ Organism at (" << pos.x << "," << pos.y
                     << ") is OUTSIDE map bounds (" << width << "x" << height << ")";
            return;
        }

        // Walidacja traversowalności kafelka
        Tile* tile = getTile(pos);
        if (tile && !tile->isTraversable()) {
            qDebug() << "  ✗ Organism at (" << pos.x << "," << pos.y
                     << ") targets an impassable tile — rejected";
            return;
        }

        // Sprawdź czy pozycja jest już zajęta — twardo odrzuć nakładanie
        for (const auto& existing : organisms) {
            if (existing->getIsAlive() && existing->getPosition() == pos) {
                qDebug() << "  ✗ Position (" << pos.x << "," << pos.y
                         << ") already occupied — organism rejected to prevent overlap";
                return;
            }
        }

        organisms.push_back(std::move(o));
        qDebug() << "  ✓ Added at (" << pos.x << "," << pos.y << ")";
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
    recentDeaths_.clear();
    for (const auto& org : organisms)
        if (!org->getIsAlive())
            recentDeaths_.push_back(org->getPosition());

    organisms.erase(
        std::remove_if(organisms.begin(), organisms.end(),
        [](const std::unique_ptr<Organism>& o) {
            return !o->getIsAlive();
        }),
        organisms.end()
    );
}
void World::setGlobalParameters(EnvironmentParameters parameters) {
    globalParameters = parameters;
}

// DODANE: ustawia zmiany z wydarzeń klimatycznych
void World::setEventModifiers(EnvironmentParameters modifiers) {
    eventModifiers = modifiers;
}

EnvironmentParameters World::getCombinedParameters(Position pos) const {
    Tile* tile = getTile(pos);
    if (tile != nullptr) {
        // ZMIENIONE: GLOBALNE + LOKALNE + WYDARZENIA = RZECZYWISTE
        return globalParameters + tile->getLocalModifiers() + eventModifiers;
    }
    // Jeśli z jakiegoś powodu jesteśmy poza mapą, zwracamy tylko globalne + wydarzenia
    return globalParameters + eventModifiers;
}
World::~World() = default;
