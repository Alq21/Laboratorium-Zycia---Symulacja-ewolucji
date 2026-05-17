#ifndef ACTIONECONOMY_H
#define ACTIONECONOMY_H

#include <algorithm>
#include <cmath>

namespace ActionEconomy {

constexpr int kUniversalMaxAp = 10;
constexpr double kEnergyPerAp = 1.5;

constexpr int kMoveSpeedWeight = 2;
constexpr int kBaseReproductionAp = 4;
constexpr double kBaseReproductionEnergy = 22.0;
constexpr int kBaseHuntAp = 1;

inline int movementApCost(int speed, int size)
{
    if (speed <= 0) {
        return kUniversalMaxAp + 1;
    }

    const int effectiveSize = std::max(1, size);
    const int effectiveSpeed = std::max(1, speed);
    return std::max(1, (effectiveSpeed * kMoveSpeedWeight + effectiveSize - 1) / effectiveSize);
}

inline int reproductionApCost(int speed, int size)
{
    return kBaseReproductionAp + (std::max(1, size) + std::max(1, speed)) / 2;
}

inline double reproductionEnergyCost(int speed, int size)
{
    const double s = static_cast<double>(std::max(1, size));
    const double v = static_cast<double>(std::max(1, speed));
    return kBaseReproductionEnergy + 3.2 * s * s + 2.6 * v * v + 1.0 * s * v;
}

inline int huntApCost(int preySize)
{
    return kBaseHuntAp + std::max(0, preySize - 1);
}


constexpr int    kLeapApCost        = 1;
constexpr double kLeapEnergyCost    = 30.0;
constexpr int    kLeapRange         = 2;
constexpr int    kLeapCooldownTicks = 4;
} // namespace ActionEconomy

#endif // ACTIONECONOMY_H