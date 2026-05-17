#include "organism.h"
#include "actioneconomy.h"
#include "lineage.h"
#include "world.h"
#include <stdlib.h>
#include "tile.h"
#include <QDebug>
#include <algorithm>
#include <cmath>
#include <vector>

namespace {
constexpr double ColdBreedTemperature = 10.0;
constexpr double WarmBreedTemperature = 30.0;
constexpr double ColdClimateThreshold = 10.0;
constexpr double WarmClimateThreshold = 30.0;
constexpr double PreferredClimateMetabolismFactor = 0.70;
}

Organism::Organism(Position pos, Color col, double startEnergy, double maxEn, int size, int speed, int /*maxAP*/, int gen,
                   TemperaturePreference tempPreference)
    : Entity(pos, col), energy(startEnergy), maxEnergy(maxEn), isAlive(true), size(size), speed(speed),
    actionPoints(ActionEconomy::kUniversalMaxAp),
    maxActionPoints(ActionEconomy::kUniversalMaxAp),
    plannedPosition(pos), generation(gen),
    lastPosition(pos), isMoving(false), temperaturePreference(tempPreference),
    lineageId(Lineage::allocateId()), parentLineageId(-1) {
}

bool Organism::isDirectKin(const Organism* other) const
{
    return Lineage::areDirectKin(this, other);
}

void Organism::initLineageFromParent(const Organism* parent)
{
    lineageId = Lineage::allocateId();
    parentLineageId = parent ? parent->getLineageId() : -1;
}

void Organism::setEnergy(double newEnergy) {
    energy = newEnergy;
    if (energy > maxEnergy) {
        energy = maxEnergy;
    }
    if (energy <= 0.0) {
        energy = 0.0;
        die();
    }
}

void Organism::die() {
    isAlive = false;
    energy = 0;
}

double Organism::climateMetabolismFactor(World* world) const {
    if (!world || temperaturePreference == TemperaturePreference::Default) {
        return 1.0;
    }

    const double temperature =
        world->getCombinedParameters(position).getTemperature();

    if (temperaturePreference == TemperaturePreference::Cryophile &&
        temperature <= ColdClimateThreshold) {
        return PreferredClimateMetabolismFactor;
    }
    if (temperaturePreference == TemperaturePreference::Thermophile &&
        temperature >= WarmClimateThreshold) {
        return PreferredClimateMetabolismFactor;
    }

    return 1.0;
}

int Organism::movementApCost() const {
    return ActionEconomy::movementApCost(speed, size);
}

int Organism::reproductionApCost() const {
    return ActionEconomy::reproductionApCost(speed, size);
}

double Organism::reproductionEnergyCost() const {
    return ActionEconomy::reproductionEnergyCost(speed, size);
}

void Organism::refillActionPoints() {
    maxActionPoints = ActionEconomy::kUniversalMaxAp;
    actionPoints = maxActionPoints;
}

bool Organism::trySpendActionPoints(int apCost) {
    if (!isAlive) {
        return false;
    }
    if (apCost <= 0) {
        return true;
    }
    if (actionPoints < apCost) {
        return false;
    }

    actionPoints -= apCost;
    setEnergy(energy - apCost * ActionEconomy::kEnergyPerAp);
    return isAlive;
}

void Organism::onTick(World* world) {
    if (!isAlive) return;

    setEnergy(energy - 1.0 * climateMetabolismFactor(world));
    if (energy <= 0) {
        die();
        return;
    }

    refillActionPoints();
}

void Organism::planMove(World* world) {
    if (!isAlive || actionPoints < movementApCost()) {
        plannedPosition = position;
        return;
    }

    int newX = position.x + (rand() % 3 - 1);
    int newY = position.y + (rand() % 3 - 1);
    plannedPosition = Position(newX, newY);
}

void Organism::onInteract(Entity* other) {
    Q_UNUSED(other);
}

bool Organism::canReproduce() const {
    if (!isAlive) {
        return false;
    }

    const int apCost = reproductionApCost();
    const double energyCost = reproductionEnergyCost();
    const double minReserve = maxEnergy * 0.76;

    return actionPoints >= apCost &&
           energy >= minReserve + energyCost;
}

Position Organism::findPlaceToBreed(World* world) const {
    if (!world) return Position{-1, -1};

    std::vector<Position> candidates;

    for (int dy = -1; dy <= 1; ++dy) {
        for (int dx = -1; dx <= 1; ++dx) {
            if (dx == 0 && dy == 0) continue;

            Position candidate{position.x + dx, position.y + dy};
            if (candidate.x < 0 || candidate.x >= world->getWidth()) continue;
            if (candidate.y < 0 || candidate.y >= world->getHeight()) continue;

            Tile* tile = world->getTile(candidate);
            if (!tile || !tile->isTraversable()) continue;
            if (world->getOrganismAt(candidate) != nullptr) continue;

            double temperature = world->getCombinedParameters(candidate).getTemperature();
            if (temperaturePreference == TemperaturePreference::Cryophile &&
                temperature > ColdBreedTemperature) {
                continue;
            }
            if (temperaturePreference == TemperaturePreference::Thermophile &&
                temperature < WarmBreedTemperature) {
                continue;
            }

            candidates.push_back(candidate);
        }
    }

    if (candidates.empty()) return Position{-1, -1};
    return candidates[rand() % candidates.size()];
}

void Organism::executeMovement(World* world) {
    if (!isAlive) return;

    const int apCost = movementApCost();
    if (actionPoints < apCost) {
        plannedPosition = position;
        lastPosition = position;
        isMoving = false;
        return;
    }

    if (position.x != plannedPosition.x || position.y != plannedPosition.y) {

        if (plannedPosition.x < 0 || plannedPosition.x >= world->getWidth() ||
            plannedPosition.y < 0 || plannedPosition.y >= world->getHeight()) {
            plannedPosition = position;
            isMoving = false;
            lastPosition = position;
            return;
        }

        Tile* targetTile = world->getTile(plannedPosition);
        if (targetTile && !targetTile->isTraversable()) {
            plannedPosition = position;
            isMoving = false;
            lastPosition = position;
            return;
        }

        Organism* occupant = world->getOrganismAt(plannedPosition);
        if (occupant != nullptr && occupant != this) {
            std::vector<Position> alternatives;

            for (int dy = -1; dy <= 1; dy++) {
                for (int dx = -1; dx <= 1; dx++) {
                    if (dx == 0 && dy == 0) continue;

                    Position alt = {position.x + dx, position.y + dy};

                    if (alt.x < 0 || alt.x >= world->getWidth() ||
                        alt.y < 0 || alt.y >= world->getHeight()) {
                        continue;
                    }

                    Tile* altTile = world->getTile(alt);
                    if (!altTile || !altTile->isTraversable()) {
                        continue;
                    }

                    if (world->getOrganismAt(alt) == nullptr) {
                        alternatives.push_back(alt);
                    }
                }
            }

            if (!alternatives.empty()) {
                int randomIndex = rand() % alternatives.size();
                plannedPosition = alternatives[randomIndex];
            } else {
                plannedPosition = position;
                isMoving = false;
                lastPosition = position;
                return;
            }
        }

        if (!trySpendActionPoints(apCost)) {
            plannedPosition = position;
            isMoving = false;
            lastPosition = position;
            return;
        }

        lastPosition = position;
        position = plannedPosition;
        isMoving = true;
    } else {
        isMoving = false;
        lastPosition = position;
    }
}

bool Organism::getIsAlive() const { return isAlive; }
double Organism::getEnergy() const { return energy; }
int Organism::getGeneration() const { return generation; }
int Organism::getSize() const { return size; }
Color Organism::getColor() const { return color; }
int Organism::getSpeed() const { return speed; }
TemperaturePreference Organism::getTemperaturePreference() const { return temperaturePreference; }
