#include "producer.h"
#include "predator.h"
#include "omnivore.h"
#include "movementplanning.h"
#include "world.h"
#include <algorithm>
#include <cstdlib>

namespace {
constexpr int TypeMutationChancePercent = 5;
constexpr double BasePhotosynthesisPerTick = 7.5;
constexpr double ProducerMetabolismPerTick = 0.5;
constexpr double ProducerReproduceEnergyFraction = 0.3;
constexpr double ChildEnergyFraction = 0.60;
constexpr double ProducerReproductionEnergyScale = 0.50;
constexpr int ProducerReproductionApDiscount = 2;

int producerReproductionApCost(const Producer& producer)
{
    return std::max(2, producer.reproductionApCost() - ProducerReproductionApDiscount);
}

double producerReproductionEnergyCost(const Producer& producer)
{
    return producer.reproductionEnergyCost() * ProducerReproductionEnergyScale;
}
}

Producer::Producer(Position pos, Color col, double startEnergy, double maxEn, int size, int speed, int maxAP, int gen,
                   TemperaturePreference tempPreference)
    : Organism(pos, col, startEnergy, maxEn, size, speed, maxAP, gen, tempPreference) {
}

void Producer::onTick(World* world) {
    if (!isAlive) return;

    setEnergy(energy - ProducerMetabolismPerTick * climateMetabolismFactor(world));
    photosynthesize(BasePhotosynthesisPerTick);
    refillActionPoints();

    if (energy <= 0.0) {
        die();
    }
}

void Producer::planMove(World* world) {
    if (!isAlive || actionPoints < movementApCost()) {
        plannedPosition = position;
        return;
    }

    plannedPosition = MovementPlanning::chooseProducerFleePosition(this, world);
    if (plannedPosition.x < 0) {
        plannedPosition = position;
    }
}

void Producer::onInteract(Entity* other) {
    (void)other;
}

bool Producer::canReproduce() const
{
    if (!isAlive) {
        return false;
    }

    const int apCost = producerReproductionApCost(*this);
    const double energyCost = producerReproductionEnergyCost(*this);
    const double minReserve = maxEnergy * ProducerReproduceEnergyFraction;

    return actionPoints >= apCost &&
           energy >= minReserve + energyCost;
}

std::unique_ptr<Organism> Producer::reproduce()
{
    if (!canReproduce()) return nullptr;

    const int apCost = producerReproductionApCost(*this);
    const double energyCost = producerReproductionEnergyCost(*this);
    if (!trySpendActionPoints(apCost)) {
        return nullptr;
    }

    energy -= energyCost;
    if (!isAlive) {
        return nullptr;
    }

    double childEnergy = energy * ChildEnergyFraction;
    energy -= childEnergy;

    Position childPos = position;

    double childMaxEn = maxEnergy;
    int childSize = size;
    Color childColor = color;

    if (rand() % 100 < 30) {
        childColor.r += (rand() % 100) - 50;
        childColor.g += (rand() % 100) - 50;
        childColor.b += (rand() % 60) - 30;

        if (childColor.r < 80) childColor.r = 80;
        if (childColor.r > 255) childColor.r = 255;
        if (childColor.g < 150) childColor.g = 150;
        if (childColor.g > 255) childColor.g = 255;
        if (childColor.b < 20) childColor.b = 20;
        if (childColor.b > 120) childColor.b = 120;

        int sizeChange = (rand() % 5) - 2;
        childSize += sizeChange;
        if (childSize < 1) childSize = 1;
        if (childSize > 6) childSize = 6;

        int energyChange = (rand() % 80) - 40;
        childMaxEn += energyChange;
        if (childMaxEn < 100) childMaxEn = 100;
        if (childMaxEn > 450) childMaxEn = 450;
    }

    if (rand() % 100 < TypeMutationChancePercent) {
        int mutantSize = childSize;
        if (mutantSize < 2) mutantSize = 2;
        if (mutantSize > 5) mutantSize = 5;

        int mutantSpeed = speed < 2 ? 2 : speed;
        int mutantVision = 5 + (rand() % 4);
        std::unique_ptr<Organism> mutant;

        if (rand() % 2 == 0) {
            mutant = std::make_unique<Predator>(
                childPos, Color{255, 50, 50}, childEnergy, childMaxEn,
                mutantSize, mutantSpeed, maxActionPoints, generation + 1,
                mutantVision, temperaturePreference);
        } else {
            mutant = std::make_unique<Omnivore>(
                childPos, Color{200, 160, 220}, childEnergy, childMaxEn,
                mutantSize, mutantSpeed, maxActionPoints, generation + 1,
                mutantVision, temperaturePreference);
        }
        mutant->initLineageFromParent(this);
        return mutant;
    }

    // Mutacja preferencji temperaturowej (5% szansy)
    TemperaturePreference childTempPref = temperaturePreference;
    if (rand() % 100 < TypeMutationChancePercent) {
        int roll = rand() % 2;
        if (temperaturePreference == TemperaturePreference::Default)
            childTempPref = (roll == 0) ? TemperaturePreference::Cryophile : TemperaturePreference::Thermophile;
        else if (temperaturePreference == TemperaturePreference::Cryophile)
            childTempPref = (roll == 0) ? TemperaturePreference::Default : TemperaturePreference::Thermophile;
        else
            childTempPref = (roll == 0) ? TemperaturePreference::Default : TemperaturePreference::Cryophile;
    }

    auto child = std::make_unique<Producer>(
        childPos,
        childColor,
        childEnergy,
        childMaxEn,
        childSize,
        speed,
        maxActionPoints,
        generation + 1,
        childTempPref
        );
    child->initLineageFromParent(this);
    return child;
}

int Producer::awarenessRadius()
{
    return MovementPlanning::producerPerceptionRadius();
}

void Producer::photosynthesize(double amount) {
    if (isAlive) {
        setEnergy(energy + amount);
        if (energy > maxEnergy) {
            energy = maxEnergy;
        }
    }
}
