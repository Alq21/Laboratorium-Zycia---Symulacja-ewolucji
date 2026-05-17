#ifndef SIMULATIONAPP_H
#define SIMULATIONAPP_H

#include <QThread>
#include <QString>
#include <memory>
#include <vector>
#include <atomic>
#include "entity.h"
// ogl tutaj cześć pozmieniałam by choDż troche gui od logiki odseparowąć, starałam sie nie usunąć za dużo twojego,możesz przejrzeć i nie potrzebne usunąć

class World;
class SimEngine;
class StatManager;
class SimulationBuilder;

class SimulationApp : public QThread
{
    Q_OBJECT

public:
    explicit SimulationApp(QObject* parent = nullptr);
    ~SimulationApp() override;

    SimulationApp(const SimulationApp&) = delete;
    SimulationApp& operator=(const SimulationApp&) = delete;


    bool loadFromFile(const QString& path);

    // Kontrola symulacji
    void startSimulation();
    void stopSimulation();
    void pauseSimulation();
    void resumeSimulation();
    void stepSimulation();  // Jeden krok

    // Dostęp do danych (dla GUI)
    std::vector<Entity*> collectEntities() const;
    double getInterpolation() const { return interpolation; }

    // Dostęp do komponentów (dla step)
    World* getWorld() const { return world.get(); }
    SimEngine* getEngine() const { return engine.get(); }

    // Status
    bool isRunning() const { return running; }
    bool isPaused() const { return paused; }
    long currentTick() const;
    QString lastError() const { return error; }

signals:
    void tickCompleted(long tick);
    void populationUpdated(int predators, int producers);
    void simulationStarted();
    void simulationPaused();
    void simulationResumed();
    void simulationStopped();
    void errorOccurred(const QString& message);

protected:
    void run() override;  // Główna pętla w wątku

private:
    // Komponenty symulacji
    std::unique_ptr<World> world;
    std::unique_ptr<SimEngine> engine;
    std::unique_ptr<StatManager> stats;
    std::unique_ptr<SimulationBuilder> builder;

    // Kontrola wątku
    std::atomic<bool> running;
    std::atomic<bool> paused;
    std::atomic<double> interpolation;

    int tickIntervalMs;
    QString error;

    void emitPopulationUpdate();
};

#endif // SIMULATIONAPP_H
