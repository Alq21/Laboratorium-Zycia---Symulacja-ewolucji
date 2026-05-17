#include "movementplanning.h"

#include "organism.h"
#include "predator.h"
#include "producer.h"
#include "tile.h"
#include "world.h"

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <vector>

namespace MovementPlanning {

namespace {

constexpr int kSmartFleeChancePercent = 45;
constexpr int kRandomPanicMoveChancePercent = 40;

const Predator* findNearestPredator(const Position& origin, World* world, int radius, int& outDistance)
{
    outDistance = radius + 1;
    const Predator* nearest = nullptr;

    if (!world) {
        return nullptr;
    }

    for (const auto& organism : world->getOrganisms()) {
        if (!organism || !organism->getIsAlive()) {
            continue;
        }

        const Predator* predator = dynamic_cast<const Predator*>(organism.get());
        if (!predator) {
            continue;
        }

        const Position predatorPos = predator->getPosition();
        const int distance = std::abs(origin.x - predatorPos.x) + std::abs(origin.y - predatorPos.y);
        if (distance <= radius && distance < outDistance) {
            outDistance = distance;
            nearest = predator;
        }
    }

    return nearest;
}

} // namespace

void gridStepDirection(double dx, double dy, int& outDx, int& outDy)
{
    if (std::abs(dx) < 1e-6 && std::abs(dy) < 1e-6) {
        outDx = 0;
        outDy = 0;
        return;
    }

    outDx = (dx > 0.25) ? 1 : ((dx < -0.25) ? -1 : 0);
    outDy = (dy > 0.25) ? 1 : ((dy < -0.25) ? -1 : 0);

    if (outDx == 0 && outDy == 0) {
        if (std::abs(dx) >= std::abs(dy)) {
            outDx = dx > 0.0 ? 1 : -1;
        } else {
            outDy = dy > 0.0 ? 1 : -1;
        }
    }
}

bool isWithinManhattanRadius(const Position& a, const Position& b, int radius)
{
    return std::abs(a.x - b.x) + std::abs(a.y - b.y) <= radius;
}

void computePredatorCenterOfMass(const Position& origin, World* world, int radius,
                                 double& centerX, double& centerY, int& predatorCount)
{
    centerX = 0.0;
    centerY = 0.0;
    predatorCount = 0;

    if (!world) {
        return;
    }

    for (const auto& organism : world->getOrganisms()) {
        if (!organism || !organism->getIsAlive()) {
            continue;
        }

        const Predator* predator = dynamic_cast<const Predator*>(organism.get());
        if (!predator) {
            continue;
        }

        const Position predatorPos = predator->getPosition();
        if (!isWithinManhattanRadius(origin, predatorPos, radius)) {
            continue;
        }

        centerX += predatorPos.x;
        centerY += predatorPos.y;
        ++predatorCount;
    }
}

void predictProducerEscapeStep(const Producer* prey, World* world, int& escapeDx, int& escapeDy)
{
    escapeDx = 0;
    escapeDy = 0;

    if (!prey || !world) {
        return;
    }

    const Position preyPos = prey->getPosition();
    int nearestDistance = kProducerAwarenessRadius + 1;
    const Predator* nearestPredator =
        findNearestPredator(preyPos, world, kProducerAwarenessRadius, nearestDistance);

    if (!nearestPredator || nearestDistance > kProducerPanicRadius) {
        return;
    }

    if (rand() % 100 >= kSmartFleeChancePercent) {
        return;
    }

    const Position threatPos = nearestPredator->getPosition();
    gridStepDirection(static_cast<double>(preyPos.x - threatPos.x),
                      static_cast<double>(preyPos.y - threatPos.y),
                      escapeDx, escapeDy);
}

static bool isValidMoveTarget(World* world, const Position& from, const Position& candidate)
{
    if (!world) {
        return false;
    }

    if (candidate.x < 0 || candidate.x >= world->getWidth() ||
        candidate.y < 0 || candidate.y >= world->getHeight()) {
        return false;
    }

    if (candidate.x == from.x && candidate.y == from.y) {
        return true;
    }

    Tile* tile = world->getTile(candidate);
    if (!tile || !tile->isTraversable()) {
        return false;
    }

    return world->getOrganismAt(candidate) == nullptr;
}

static std::vector<Position> collectNeighborCandidates(const Position& origin)
{
    std::vector<Position> candidates;
    candidates.reserve(9);

    for (int dy = -1; dy <= 1; ++dy) {
        for (int dx = -1; dx <= 1; ++dx) {
            candidates.push_back(Position{origin.x + dx, origin.y + dy});
        }
    }

    return candidates;
}

static std::vector<Position> collectValidNeighbors(World* world, const Position& origin)
{
    std::vector<Position> valid;
    for (const Position& candidate : collectNeighborCandidates(origin)) {
        if (isValidMoveTarget(world, origin, candidate)) {
            valid.push_back(candidate);
        }
    }
    return valid;
}

static Position pickRandomNeighbor(World* world, const Position& origin)
{
    const std::vector<Position> valid = collectValidNeighbors(world, origin);
    if (valid.empty()) {
        return origin;
    }
    return valid[rand() % valid.size()];
}

Position chooseProducerFleePosition(const Producer* producer, World* world)
{
    if (!producer || !world) {
        return Position{-1, -1};
    }

    const Position origin = producer->getPosition();
    int nearestDistance = kProducerAwarenessRadius + 1;
    const Predator* nearestPredator =
        findNearestPredator(origin, world, kProducerAwarenessRadius, nearestDistance);

    if (!nearestPredator || nearestDistance > kProducerPanicRadius) {
        return pickRandomNeighbor(world, origin);
    }

    if (rand() % 100 < kRandomPanicMoveChancePercent ||
        rand() % 100 >= kSmartFleeChancePercent) {
        return pickRandomNeighbor(world, origin);
    }

    const Position threatPos = nearestPredator->getPosition();
    const std::vector<Position> valid = collectValidNeighbors(world, origin);
    Position best = origin;
    int bestDistance = std::abs(origin.x - threatPos.x) + std::abs(origin.y - threatPos.y);

    for (const Position& candidate : valid) {
        const int distance =
            std::abs(candidate.x - threatPos.x) + std::abs(candidate.y - threatPos.y);
        if (distance > bestDistance || (distance == bestDistance && rand() % 2 == 0)) {
            bestDistance = distance;
            best = candidate;
        }
    }

    return best;
}

Position choosePredatorPursuitPosition(const Organism* predator, const Producer* prey, World* world)
{
    if (!predator || !prey || !world) {
        return Position{-1, -1};
    }

    const Position origin = predator->getPosition();
    const Position preyPos = prey->getPosition();

    int escapeDx = 0;
    int escapeDy = 0;
    predictProducerEscapeStep(prey, world, escapeDx, escapeDy);

    const Position estimatedPreyPos{preyPos.x + escapeDx, preyPos.y + escapeDy};

    int pursuitDx = 0;
    int pursuitDy = 0;
    gridStepDirection(static_cast<double>(estimatedPreyPos.x - origin.x),
                      static_cast<double>(estimatedPreyPos.y - origin.y),
                      pursuitDx, pursuitDy);

    const Position preferred{origin.x + pursuitDx, origin.y + pursuitDy};
    if (isValidMoveTarget(world, origin, preferred)) {
        return preferred;
    }

    const std::vector<Position> candidates = collectNeighborCandidates(origin);
    Position best = origin;
    double bestScore = -1e9;

    for (const Position& candidate : candidates) {
        if (!isValidMoveTarget(world, origin, candidate)) {
            continue;
        }

        const double dx = candidate.x - estimatedPreyPos.x;
        const double dy = candidate.y - estimatedPreyPos.y;
        const double distSq = dx * dx + dy * dy;
        const double score = -distSq;

        if (score > bestScore) {
            bestScore = score;
            best = candidate;
        }
    }

    return best;
}

Position chooseHunterPursuitPosition(const Organism* hunter, const Organism* prey, World* world)
{
    if (!hunter || !prey || !world) {
        return Position{-1, -1};
    }

    if (const Producer* producerPrey = dynamic_cast<const Producer*>(prey)) {
        return choosePredatorPursuitPosition(hunter, producerPrey, world);
    }

    const Position origin = hunter->getPosition();
    const Position preyPos = prey->getPosition();

    int pursuitDx = 0;
    int pursuitDy = 0;
    gridStepDirection(static_cast<double>(preyPos.x - origin.x),
                      static_cast<double>(preyPos.y - origin.y),
                      pursuitDx, pursuitDy);

    const Position preferred{origin.x + pursuitDx, origin.y + pursuitDy};
    if (isValidMoveTarget(world, origin, preferred)) {
        return preferred;
    }

    return pickRandomNeighbor(world, origin);
}

}
