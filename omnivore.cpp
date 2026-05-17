#include "omnivore.h"
#include "producer.h"
#include "predator.h"
#include "actioneconomy.h"
#include "movementplanning.h"
#include "world.h"

#include <algorithm>
#include <cstdlib>

namespace {
constexpr int TypeMutationChancePercent = 5;
constexpr double ChildEnergyFraction = 0.42;
constexpr double OmnivoreMetabolismPerTick = 2.0;
constexpr double OmnivoreReproduceThreshold = 80.0;
constexpr double HuntEnergyDivisor = 1.38;
constexpr double OmnivoreReproductionEnergyScale = 0.78;
constexpr int OmnivoreReproductionApDiscount = 2;

int omnivoreReproductionApCost(const Omnivore& omnivore)
{
    return std::max(2, omnivore.reproductionApCost() - OmnivoreReproductionApDiscount);
}

double omnivoreReproductionEnergyCost(const Omnivore& omnivore)
{
    return omnivore.reproductionEnergyCost() * OmnivoreReproductionEnergyScale;
}
}

Omnivore::Omnivore(Position pos, Color col, double startEnergy, double maxEn, int size, int speed, int maxAP, int gen,
                   int vision, TemperaturePreference tempPreference)
    : Organism(pos, col, startEnergy, maxEn, size, speed, maxAP, gen, tempPreference), visionRange(vision) {
}

int Omnivore::getVision() const
{
    return std::max(visionRange, MovementPlanning::minimumPredatorVisionRadius());
}

Organism* Omnivore::findNearestPrey(World* world) const
{
    if (!world) {
        return nullptr;
    }

    Organism* bestPrey = nullptr;
    const int vision = getVision();
    double minDistanceSq = (vision * vision) + 1.0;

    for (const auto& org : world->getOrganisms()) {
        if (!org || !org->getIsAlive() || org.get() == this) {
            continue;
        }

        Organism* candidate = org.get();
        if (isDirectKin(candidate)) {
            continue;
        }

        const Position preyPos = candidate->getPosition();
        const double dx = preyPos.x - position.x;
        const double dy = preyPos.y - position.y;
        const double distSq = (dx * dx) + (dy * dy);

        if (distSq <= (vision * vision) && distSq < minDistanceSq) {
            minDistanceSq = distSq;
            bestPrey = candidate;
        }
    }

    return bestPrey;
}

void Omnivore::onTick(World* world)
{
    if (!isAlive) {
        return;
    }

    setEnergy(energy - OmnivoreMetabolismPerTick * climateMetabolismFactor(world));
    refillActionPoints();

    if (energy <= 0) {
        die();
    }
}

void Omnivore::planMove(World* world)
{
    if (!isAlive || actionPoints < movementApCost()) {
        plannedPosition = position;
        return;
    }

    Organism* prey = findNearestPrey(world);
    if (prey) {
        plannedPosition = MovementPlanning::chooseHunterPursuitPosition(this, prey, world);
        if (plannedPosition.x < 0) {
            plannedPosition = position;
        }
        return;
    }

    int dx = (rand() % 3) - 1;
    int dy = (rand() % 3) - 1;
    plannedPosition = Position{position.x + dx, position.y + dy};
}

void Omnivore::onInteract(Entity* other)
{
    if (!isAlive) {
        return;
    }

    Organism* prey = dynamic_cast<Organism*>(other);
    if (prey && prey->getIsAlive()) {
        attemptHunt(prey);
    }
}

bool Omnivore::attemptHunt(Organism* target)
{
    if (!target || !target->getIsAlive() || target == this || isDirectKin(target)) {
        return false;
    }

    const int apCost = ActionEconomy::huntApCost(target->getSize());
    if (!trySpendActionPoints(apCost)) {
        return false;
    }

    const double stolenEnergy = target->getEnergy() / HuntEnergyDivisor;
    target->setEnergy(target->getEnergy() - stolenEnergy);
    setEnergy(energy + stolenEnergy);
    return isAlive;
}

bool Omnivore::canReproduce() const
{
    if (!isAlive) {
        return false;
    }

    const int apCost = omnivoreReproductionApCost(*this);
    const double energyCost = omnivoreReproductionEnergyCost(*this);

    return actionPoints >= apCost &&
           energy >= OmnivoreReproduceThreshold + energyCost;
}

std::unique_ptr<Organism> Omnivore::reproduce()
{
    if (!canReproduce()) {
        return nullptr;
    }

    const int apCost = omnivoreReproductionApCost(*this);
    const double energyCost = omnivoreReproductionEnergyCost(*this);
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
    int childVision = visionRange;
    int childSpeed = speed;
    int childSize = size;
    Color childColor = color;

    if (rand() % 100 < 25) {
        childColor.r = 150 + (rand() % 80);
        childColor.g = 100 + (rand() % 80);
        childColor.b = 150 + (rand() % 80);

        int sizeChange = (rand() % 3) - 1;
        childSize += sizeChange;
        if (childSize < 2) childSize = 2;
        if (childSize > 5) childSize = 5;

        int visionChange = (rand() % 5) - 2;
        childVision += visionChange;
        if (childVision < 3) childVision = 3;
        if (childVision > 15) childVision = 15;
    }

    if (rand() % 100 < TypeMutationChancePercent) {
        const int roll = rand() % 3;
        std::unique_ptr<Organism> mutant;
        if (roll == 0) {
            mutant = std::make_unique<Producer>(
                childPos, Color{255, 220, 50}, childEnergy, childMaxEn,
                childSize, std::max(1, childSpeed - 1), maxActionPoints,
                generation + 1, temperaturePreference);
        } else if (roll == 1) {
            mutant = std::make_unique<Predator>(
                childPos, Color{255, 50, 50}, childEnergy, childMaxEn,
                childSize, childSpeed, maxActionPoints, generation + 1,
                childVision, temperaturePreference);
        } else {
            mutant = std::make_unique<Omnivore>(
                childPos, Color{200, 160, 220}, childEnergy, childMaxEn,
                childSize, childSpeed, maxActionPoints, generation + 1,
                childVision, temperaturePreference);
        }
        mutant->initLineageFromParent(this);
        return mutant;
    }

    auto child = std::make_unique<Omnivore>(
        childPos, childColor, childEnergy, childMaxEn,
        childSize, childSpeed, maxActionPoints, generation + 1,
        childVision, temperaturePreference);
    child->initLineageFromParent(this);
    return child;
}
