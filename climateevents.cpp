/**
 * @file climateevents.cpp
 * @brief Implementacja systemu wydarzeń klimatycznych
 */

#include "climateevents.h"
#include <algorithm>

// ========== ClimateEvent (struktura) ==========

bool ClimateEvent::isActive() const {
    return ticksLeft > 0;
}

void ClimateEvent::start() {
    ticksLeft = duration;
}

void ClimateEvent::update() {
    if (ticksLeft > 0) {
        ticksLeft--;
    }
}

EnvironmentParameters ClimateEvent::getModifiers() const {
    return EnvironmentParameters(tempMod, humMod, sunMod);
}

// ========== ClimateEvents (klasa) ==========

ClimateEvents::ClimateEvents()
    : autoEnabled(true),
      cooldown(0)
{
}

void ClimateEvents::update() {
    // Aktualizuj istniejące wydarzenia
    for (auto& event : activeEvents) {
        event.update();
    }
    
    // Usuń zakończone wydarzenia
    activeEvents.erase(
        std::remove_if(activeEvents.begin(), activeEvents.end(),
            [](const ClimateEvent& e) { return !e.isActive(); }),
        activeEvents.end()
    );
    
    // Zmniejsz cooldown
    if (cooldown > 0) {
        cooldown--;
    }
    
}

void ClimateEvents::trigger(ClimateEventType type) {
    ClimateEvent event;
    event.type = type;
    event.duration = 10;  // Domyślny czas trwania
    
    switch (type) {
        case ClimateEventType::Heatwave:
            event.name = "Fala upałów";
            event.tempMod = 18.0;
            event.humMod = -25.0;
            event.sunMod = 15.0;
            break;
            
        case ClimateEventType::Freeze:
            event.name = "Mróz";
            event.tempMod = -18.0;
            event.humMod = 12.0;
            event.sunMod = -25.0;
            break;
            
        case ClimateEventType::Drought:
            event.name = "Susza";
            event.tempMod = 8.0;
            event.humMod = -35.0;
            event.sunMod = 20.0;
            event.duration = 70;  // Susza trwa dłużej
            break;
            
        case ClimateEventType::Storm:
            event.name = "Burza";
            event.tempMod = -6.0;
            event.humMod = 35.0;
            event.sunMod = -45.0;
            event.duration = 30;  // Burza trwa krócej
            break;
            
        default:
            return;  // Nie dodawaj wydarzenia typu None
    }
    
    event.start();
    activeEvents.push_back(event);
}

// void ClimateEvents::triggerRandom() {
//     // Losuj typ wydarzenia (1-4)
//     int randomType = (rand() % 4) + 1;
//     trigger(static_cast<ClimateEventType>(randomType));
// }

EnvironmentParameters ClimateEvents::getTotalModifiers() const {
    EnvironmentParameters total(0.0, 0.0, 0.0);
    
    for (const auto& event : activeEvents) {
        if (event.isActive()) {
            total = total + event.getModifiers();
        }
    }
    
    return total;
}

std::vector<std::string> ClimateEvents::getActiveNames() const {
    std::vector<std::string> names;
    
    for (const auto& event : activeEvents) {
        if (event.isActive()) {
            names.push_back(event.name);
        }
    }
    
    return names;
}

// void ClimateEvents::setAutoEnabled(bool enabled) {
//     autoEnabled = enabled;
// }

// bool ClimateEvents::isAutoEnabled() const {
//     return autoEnabled;
// }
