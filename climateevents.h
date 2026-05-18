/**
 * @file climateevents.h
 * @brief System wydarzeń klimatycznych dla symulacji
 */

#ifndef CLIMATEEVENTS_H
#define CLIMATEEVENTS_H

#include "environmentparameters.h"
#include <string>
#include <vector>
#include <cstdlib>  // dla rand() i srand()

/**
 * @brief Typy wydarzeń klimatycznych
 */
enum class ClimateEventType {
    None,
    Heatwave,    // Fala upałów
    Freeze,      // Mróz
    Drought,     // Susza
    Storm        // Burza
};

/**
 * @brief Jedno wydarzenie klimatyczne - prosta struktura
 */
struct ClimateEvent {
    ClimateEventType type = ClimateEventType::None;
    std::string name = "";
    int duration = 0;           // Ile ticków trwa
    int ticksLeft = 0;          // Ile zostało
    double tempMod = 0.0;       // Modyfikator temperatury
    double humMod = 0.0;        // Modyfikator wilgotności
    double sunMod = 0.0;        // Modyfikator nasłonecznienia
    
    bool isActive() const;
    void start();
    void update();
    EnvironmentParameters getModifiers() const;
};

/**
 * @brief Menedżer wydarzeń klimatycznych
 */
class ClimateEvents {
private:
    std::vector<ClimateEvent> activeEvents;
    bool autoEnabled;
    int cooldown;

public:
    ClimateEvents();
    
    /**
     * @brief Aktualizuj wydarzenia (wywołaj co tick)
     */
    void update();
    
    /**
     * @brief Wywołaj konkretne wydarzenie
     * @param type Typ wydarzenia do wywołania
     */
    void trigger(ClimateEventType type);
    
    /**
     * @brief Wywołaj losowe wydarzenie
     */
    // void triggerRandom();
    
    /**
     * @brief Pobierz łączne modyfikatory ze wszystkich aktywnych wydarzeń
     * @return Zsumowane parametry środowiska
     */
    EnvironmentParameters getTotalModifiers() const;
    
    /**
     * @brief Pobierz nazwy aktywnych wydarzeń
     * @return Wektor nazw aktywnych wydarzeń
     */
    std::vector<std::string> getActiveNames() const;
    
    /**
     * @brief Włącz/wyłącz automatyczne wydarzenia
     * @param enabled true = włączone, false = wyłączone
     */
    // void setAutoEnabled(bool enabled);
    
    /**
     * @brief Sprawdź czy automatyczne wydarzenia są włączone
     * @return true jeśli włączone
     */
//     bool isAutoEnabled() const;
 };

#endif // CLIMATEEVENTS_H
