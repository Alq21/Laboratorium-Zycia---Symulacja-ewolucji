// #include "thermophile.h"

// #include "environmentparameters.h"
// #include "thermophile.h"
// #include "world.h"
// #include <cmath>

// Thermophile::Thermophile(Position pos, Color col, double startEnergy, double maxEn, int startSize, int startSpeed, int maxAP, int gen, double temp)
//     : Producer(pos, col, startEnergy, maxEn, startSize, startSpeed, maxAP, gen) {
// }

// void Thermophile::onTick(World* world) {
//     if (!isAlive) return;

//     // PARAMETRY ŚRODOWISKA (Globalne + Lokalne kafelka)
//     EnvironmentParameters currentConditions = world->getCombinedParameters(position);

//     double currentTemp = currentConditions.getTemperature();
//     double tempDifference = std::abs(currentTemp - preferredTemperature);

//     if (tempDifference <= 10.0) {
//         setEnergy(energy + 5.0);
//     } else {
//         setEnergy(energy - (tempDifference * 0.2));
//     }



//     Producer::onTick(world);
// }

// std::unique_ptr<Organism> Thermophile::reproduce()
// {
//     if (!canReproduce()) return nullptr;


//     double childEnergy = energy * 0.4;
//     energy -= childEnergy;

//     Position childPos = position;
//     childPos.x += 1;


//     double childMaxEn = maxEnergy;
//     int childSize = size;
//     Color childColor = color;


//     if (rand() % 100 < 30) {
//         // Mutacja kolor
//         childColor.r = 200 + (rand() % 56);
//         childColor.g = 100 + (rand() % 100);
//         childColor.b = 20 + (rand() % 80);

//         // Mutacja rozmiaru
//         int sizeChange = (rand() % 5) - 2;
//         childSize += sizeChange;
//         if (childSize < 1) childSize = 1;
//         if (childSize > 6) childSize = 6;

//         // Mutacja energii
//         int energyChange = (rand() % 80) - 40;
//         childMaxEn += energyChange;
//         if (childMaxEn < 100) childMaxEn = 100;
//         if (childMaxEn > 450) childMaxEn = 450;

//     }

//     return std::make_unique<Thermophile>(
//         childPos,
//         childColor,
//         childEnergy,
//         childMaxEn,
//         childSize,
//         speed,
//         maxActionPoints,
//         generation + 1
//         );
// }




// void Thermophile::planMove(World* world) {
//     // to ogólnie bedzie działać jak dodamy płytki tak samo w crypthophile
//     if (!isAlive || actionPoints <= 0) return;

//     double bestTempDifference = 9999.0;
//     Position bestPosition = position;

//     std::vector<Position> equallyGoodPositions;

//     for (int dx = -1; dx <= 1; ++dx) {
//         for (int dy = -1; dy <= 1; ++dy) {
//             Position checkPos = {position.x + dx, position.y + dy};

//             if (world->getTile(checkPos) != nullptr) {
//                 EnvironmentParameters params = world->getCombinedParameters(checkPos);
//                 double currentTemp = params.getTemperature();

//                 double tempDifference = std::abs(currentTemp - preferredTemperature);

//                 if (tempDifference < bestTempDifference) {
//                     bestTempDifference = tempDifference;
//                     equallyGoodPositions.clear();
//                     equallyGoodPositions.push_back(checkPos);
//                 } else if (tempDifference == bestTempDifference) {
//                     equallyGoodPositions.push_back(checkPos);
//                 }
//             }
//         }
//     }

//     if (!equallyGoodPositions.empty()) {
//         int randomIndex = rand() % equallyGoodPositions.size();
//         plannedPosition = equallyGoodPositions[randomIndex];
//     } else {
//         plannedPosition = position;
//     }
// }
