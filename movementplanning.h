#ifndef MOVEMENTPLANNING_H
#define MOVEMENTPLANNING_H

#include "position.h"

class World;
class Organism;
class Producer;

namespace MovementPlanning {

constexpr int kProducerAwarenessRadius = 3;
constexpr int kProducerPanicRadius = 2;
constexpr int kPredatorVisionBonusOverProducer = 2;

inline int producerPerceptionRadius() { return kProducerAwarenessRadius; }
inline int minimumPredatorVisionRadius() {
    return kProducerAwarenessRadius + kPredatorVisionBonusOverProducer;
}

void gridStepDirection(double dx, double dy, int& outDx, int& outDy);
bool isWithinManhattanRadius(const Position& a, const Position& b, int radius);
void computePredatorCenterOfMass(const Position& origin, World* world, int radius,
                                 double& centerX, double& centerY, int& predatorCount);
void predictProducerEscapeStep(const Producer* prey, World* world, int& escapeDx, int& escapeDy);
Position chooseProducerFleePosition(const Producer* producer, World* world);
Position choosePredatorPursuitPosition(const Organism* predator, const Producer* prey, World* world);
Position chooseHunterPursuitPosition(const Organism* hunter, const Organism* prey, World* world);

} // namespace MovementPlanning

#endif // MOVEMENTPLANNING_H
