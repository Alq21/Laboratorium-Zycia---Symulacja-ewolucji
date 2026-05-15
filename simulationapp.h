#ifndef SIMULATIONAPP_H
#define SIMULATIONAPP_H

#include <QObject>
#include <QString>
#include <memory>
#include <vector>
#include "entity.h"

class QTimer;
class World;
class SimEngine;
class StatManager;
class SimulationBuilder;

class SimulationApp : public QObject {
    Q_OBJECT

public:
    explicit SimulationApp(QObject* parent = nullptr);
    ~SimulationApp() override;

    SimulationApp(const SimulationApp&) = delete;
    SimulationApp& operator=(const SimulationApp&) = delete;

    bool loadFromFile(const QString& path);
    void setTickInterval(int ms);
    void start();
    void pause();
    void resume();
    void stop();
    //trzeba dodać step();
    void spawnEntity(int x, int y, const QString& type);
    bool isRunning() const;
    bool isPaused()  const;
    long currentTick() const;

    std::vector<Entity*> collectEntities() const;

    QString lastError() const { return error; }

signals:
    void tickCompleted(long tick);
    void populationUpdated(int predators, int producers);
    void simulationStarted();
    void simulationPaused();
    void simulationResumed();
    void simulationStopped();
    void errorOccurred(const QString& message);

private slots:
    void onTick();

private:
    std::unique_ptr<World> world;
    std::unique_ptr<SimEngine> engine;
    std::unique_ptr<StatManager> stats;
    std::unique_ptr<SimulationBuilder> builder;
    QTimer* tickTimer = nullptr;

    int tickIntervalMs = 100;
    bool running = false;
    bool paused = false;
    QString error;

    void tearDown();
    void emitPopulationUpdate();
};

#endif // SIMULATIONAPP_H
