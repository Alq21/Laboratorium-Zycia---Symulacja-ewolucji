#include "simulationbuilder.h"
#include "world.h"
#include "predator.h"
#include "producer.h"
#include "thermophile.h"
#include "cryophile.h"
#include "entity.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

static MapConfig parseMapConfig(const QJsonObject& obj) {
    MapConfig cfg;
    if (obj.contains("baseNormalWeight")) cfg.baseNormalWeight = obj["baseNormalWeight"].toInt(cfg.baseNormalWeight);
    if (obj.contains("baseAbundantWeight")) cfg.baseAbundantWeight = obj["baseAbundantWeight"].toInt(cfg.baseAbundantWeight);
    if (obj.contains("basePoisonWeight")) cfg.basePoisonWeight = obj["basePoisonWeight"].toInt(cfg.basePoisonWeight);
    if (obj.contains("baseImpassableWeight")) cfg.baseImpassableWeight = obj["baseImpassableWeight"].toInt(cfg.baseImpassableWeight);
    if (obj.contains("clusterBonus")) cfg.clusterBonus = obj["clusterBonus"].toInt(cfg.clusterBonus);
    return cfg;
}

// Parse one organism entry from the "organisms" JSON array.
static OrganismSpawnConfig parseOrganism(const QJsonObject& obj) {
    OrganismSpawnConfig cfg;
    cfg.type = obj["type"].toString("producer").toStdString();
    cfg.x = obj["x"].toInt(0);
    cfg.y = obj["y"].toInt(0);
    cfg.startEnergy = obj["startEnergy"].toDouble(cfg.startEnergy);
    cfg.maxEnergy = obj["maxEnergy"].toDouble(cfg.maxEnergy);
    cfg.size = obj["size"].toInt(cfg.size);
    cfg.speed = obj["speed"].toInt(cfg.speed);
    cfg.maxAP = obj["maxAP"].toInt(cfg.maxAP);
    cfg.vision = obj["vision"].toInt(cfg.vision);
    cfg.preferredTemperature = obj["preferredTemperature"].toDouble(cfg.preferredTemperature);
    return cfg;
}

std::unique_ptr<Organism> SimulationBuilder::makeOrganism(const OrganismSpawnConfig& cfg) {
    Position pos(cfg.x, cfg.y);

    if (cfg.type == "predator") {
        return std::make_unique<Predator>(
            pos,
            Color{255, 50, 50},
            cfg.startEnergy,
            cfg.maxEnergy,
            cfg.size,
            cfg.speed,
            cfg.maxAP,
            0,
            cfg.vision
            );
    }

    if (cfg.type == "producer") {
        return std::make_unique<Producer>(
            pos,
            Color{50, 200, 50},
            cfg.startEnergy,
            cfg.maxEnergy,
            cfg.size,
            cfg.speed,
            cfg.maxAP,
            0,
            cfg.preferredTemperature
            );
    }

    if (cfg.type == "thermophile") {
        return std::make_unique<Thermophile>(
            pos,
            Color{255, 100, 50},
            cfg.startEnergy,
            cfg.maxEnergy,
            cfg.size,
            cfg.speed,
            cfg.maxAP,
            0
            );
    }

    if (cfg.type == "cryophile") {
        return std::make_unique<Cryophile>(
            pos,
            Color{50, 150, 255},
            cfg.startEnergy,
            cfg.maxEnergy,
            cfg.size,
            cfg.speed,
            cfg.maxAP,
            0
            );
    }

    error = QString("SimulationBuilder: unknown organism type")
                .arg(QString::fromStdString(cfg.type));
    return nullptr;
}


WorldConfig SimulationBuilder::loadConfig(const QString& path) {
    error.clear();
    WorldConfig config;

    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        error = QString("SimulationBuilder: cannot open config file ").arg(path);
        return config;
    }

    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll(), &parseError);
    file.close();

    if (doc.isNull()) {
        error = QString("SimulationBuilder: JSON parse error").arg(parseError.errorString());
        return config;
    }

    QJsonObject root = doc.object();
    if (root.contains("world")) {
        QJsonObject worldObj = root["world"].toObject();
        config.width     = worldObj["width"].toInt(config.width);
        config.height    = worldObj["height"].toInt(config.height);
        if (worldObj.contains("mapConfig"))
            config.mapConfig = parseMapConfig(worldObj["mapConfig"].toObject());
    }
    if (root.contains("organisms")) {
        for (const QJsonValue& val : root["organisms"].toArray())
            config.organisms.push_back(parseOrganism(val.toObject()));
    }
    qDebug() << " LOADING CONFIG ";
    qDebug() << "File:" << path;
    if (root.contains("organisms")) {
        QJsonArray orgsArray = root["organisms"].toArray();
        qDebug() << "Found" << orgsArray.size() << "organisms in JSON";
    }
    qDebug() << "Config loaded. Total organisms:" << config.organisms.size();
    return config;
}

    std::unique_ptr<World> SimulationBuilder::buildWorld(const WorldConfig& config) {
        qDebug() << "\n=== BUILDING WORLD ===";
        qDebug() << "Map size:" << config.width << "x" << config.height;
        qDebug() << "Organisms to spawn:" << config.organisms.size();

        auto world = std::make_unique<World>(config.width, config.height, config.mapConfig);

        int spawned = 0;
        for (const auto& spawnCfg : config.organisms) {
            qDebug() << "\nSpawning organism #" << (spawned+1);
            qDebug() << "  Type:" << QString::fromStdString(spawnCfg.type);
            qDebug() << "  Position: (" << spawnCfg.x << "," << spawnCfg.y << ")";
            qDebug() << "  Energy:" << spawnCfg.startEnergy << "/" << spawnCfg.maxEnergy;
            qDebug() << "  Size:" << spawnCfg.size;

            auto organism = makeOrganism(spawnCfg);
            if (organism) {
                world->addOrganism(std::move(organism));
                spawned++;
                qDebug() << "  ✓ SUCCESS";
            } else {
                qDebug() << "  ✗ FAILED - makeOrganism returned nullptr";
            }
        }

        qDebug() << "\n=== SPAWN SUMMARY ===";
        qDebug() << "Requested:" << config.organisms.size();
        qDebug() << "Spawned:" << spawned;
        qDebug() << "In world:" << world->getOrganisms().size();

        return world;
    }


// std::unique_ptr<World> SimulationBuilder::buildWorld(const WorldConfig& config) {
//     auto world = std::make_unique<World>(config.width, config.height, config.mapConfig);

//     for (const auto& spawnCfg : config.organisms) {
//         auto organism = makeOrganism(spawnCfg);
//         if (organism)
//             world->addOrganism(std::move(organism));
//     }

//     return world;
// }

void SimulationBuilder::queueSpawn(int x, int y, const QString& type) {
    OrganismSpawnConfig cfg;
    cfg.type = type.toLower().toStdString();
    cfg.x    = x;
    cfg.y    = y;

    auto organism = makeOrganism(cfg);
    if (organism)
        spawnQueue.push(std::move(organism));
}

void SimulationBuilder::flushSpawnQueue(World* world) {
    if (!world) return;
    while (!spawnQueue.empty()) {
        world->addOrganism(std::move(spawnQueue.front()));
        spawnQueue.pop();
    }
}
