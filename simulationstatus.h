#ifndef SIMULATIONSTATUS_H
#define SIMULATIONSTATUS_H

#include <string>

enum class SimulationState {
    INITIALIZING,
    RUNNING,
    PAUSED,
    STOPPED,
    FINISHED
};

class SimulationStatus {
private:
    SimulationState state;

public:
    SimulationStatus();

    void setState(SimulationState s) { state = s; }
    SimulationState getState() const { return state; }

    bool isRunning() const { return state == SimulationState::RUNNING; }
    bool isPaused() const { return state == SimulationState::PAUSED; }

    std::string toString() const {
        switch (state) {
        case SimulationState::INITIALIZING: return "Initializing";
        case SimulationState::RUNNING:      return "Running";
        case SimulationState::PAUSED:       return "Paused";
        case SimulationState::STOPPED:      return "Stopped";
        case SimulationState::FINISHED:     return "Finished";
        default:                            return "Unknown";
        }
    }
};

#endif // SIMULATIONSTATUS_H