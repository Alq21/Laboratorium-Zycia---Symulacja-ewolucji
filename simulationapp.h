#ifndef SIMULATIONAPP_H
#define SIMULATIONAPP_H

#include <QMutex>
#include <QThread>
#include <QString>
#include <memory>
#include <vector>
#include <atomic>
#include "boardsnapshot.h"
#include "entity.h"

class World;
class SimEngine;
class StatManager;
class SimulationBuilder;

// Dane statystyk zwracane bezpiecznie (kopie spod muteksa)
struct StatsData {
    std::vector<long>   ticks;
    std::vector<int>    predatorCounts;
    std::vector<int>    producerCounts;
    std::vector<int>    omnivoreCounts;
    std::vector<int>    cryophileCounts;
    std::vector<int>    thermophileCounts;
    std::vector<int>    defaultCounts;
    std::vector<double> avgSizes;
    std::vector<double> avgSpeeds;
    bool hasData = false;
};

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
    void restartSimulation(const QString& configPath);
    void pauseSimulation();
    void resumeSimulation();
    void stepSimulation();

    // Dostep do danych (dla GUI)
    BoardSnapshot collectSnapshot() const;
    double getInterpolation() const { return interpolation.load(); }

    // Dane statystyk — bezpieczna kopia spod muteksa
    StatsData collectStats() const;

    // Status
    bool isRunning() const { return running; }
    bool isPaused()  const { return paused;  }
    long currentTick() const;
    QString lastError() const { return error; }
    
    // DODANE: Dostęp do silnika (dla GUI)
    SimEngine* getEngine() const;

signals:
    void tickCompleted(long tick);
    void populationUpdated(int predators, int producers);
    void simulationStarted();
    void simulationPaused();
    void simulationResumed();
    void simulationStopped();
    void errorOccurred(const QString& message);

protected:
    void run() override;

private:
    std::unique_ptr<World>             world;
    std::unique_ptr<SimEngine>         engine;
    std::unique_ptr<StatManager>       stats;
    std::unique_ptr<SimulationBuilder> builder;

    std::atomic<bool>   running;
    std::atomic<bool>   paused;
    std::atomic<double> interpolation;

    int     tickIntervalMs;
    QString error;
    QString loadedConfigPath;
    mutable QMutex worldMutex;
};

#endif // SIMULATIONAPP_H
