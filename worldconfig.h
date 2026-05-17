#ifndef WORLDCONFIG_H
#define WORLDCONFIG_H

#include <string>
#include <vector>
#include "temperaturepreference.h"
#include "world.h"

struct OrganismSpawnConfig {
    std::string type;
    int x = 0;
    int y = 0;
    double startEnergy = 80.0;
    double maxEnergy = 200.0;
    int    size = 1;
    int    speed = 1;
    int    maxAP = 1;
    int    vision = 5;
    TemperaturePreference temperaturePreference = TemperaturePreference::Default;
};

struct WorldConfig {
    int width = 100;
    int height = 100;
    MapConfig mapConfig;
    std::vector<OrganismSpawnConfig> organisms;
};

#endif // WORLDCONFIG_H
