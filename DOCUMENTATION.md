/**
 * @mainpage Laboratorium Życia - Dokumentacja Projektu
 * 
 * @section intro_sec Wprowadzenie
 * 
 * **Laboratorium Życia** to symulacja ekologiczna napisana w C++ z interfejsem Qt,
 * która modeluje ewolucję i interakcje organizmów w dynamicznym środowisku.
 * 
 * @section features_sec Główne funkcje
 * 
 * - **Różnorodne organizmy**: Producenci (rośliny), Drapieżniki, Wszystkożercy
 * - **Preferencje temperaturowe**: Kryofile (zimno), Termofile (ciepło), Domyślne
 * - **Wydarzenia klimatyczne**: Fale upałów, Mrozy, Susze, Burze
 * - **Dynamiczne środowisko**: Płytki z różnymi właściwościami (normalne, obfite, trujące, nieprzejezdne)
 * - **Kontrola parametrów**: Temperatura, wilgotność, nasłonecznienie
 * - **Statystyki w czasie rzeczywistym**: Wykresy populacji, średnich cech
 * 
 * @section arch_sec Architektura
 * 
 * ### Główne moduły:
 * 
 * #### 1. Silnik symulacji (SimEngine)
 * - Koordynuje cykl życia symulacji
 * - Przetwarza fazy ticku w sekwencji
 * - Zarządza wydarzeniami klimatycznymi
 * 
 * #### 2. Świat (World)
 * - Przechowuje siatkę płytek (Tile)
 * - Zarządza kolekcją organizmów (Organism)
 * - Oblicza łączne parametry środowiska
 * 
 * #### 3. Organizmy (Organism hierarchia)
 * - **Organism** - klasa bazowa dla wszystkich żywych stworzeń
 * - **Producer** - rośliny, czerpią energię z płytek
 * - **Predator** - drapieżniki, polują na inne organizmy
 * - **Omnivore** - wszystkożercy, mogą jeść rośliny i zwierzęta
 * - **Cryophile/Thermophile** - specjalizacje temperaturowe
 * 
 * #### 4. Płytki (Tile hierarchia)
 * - **Tile** - klasa bazowa dla terenu
 * - **NormalTile** - standardowa płytka
 * - **AbundantTile** - daje dodatkową energię
 * - **PoisonTile** - zabiera energię
 * - **ImpassableTile** - przeszkoda
 * 
 * #### 5. Wydarzenia klimatyczne (ClimateEvent)
 * - **ClimateEventManager** - zarządza wydarzeniami
 * - **Heatwave** - fala upałów (+temperatura, -wilgotność)
 * - **Freeze** - mróz (-temperatura, +wilgotność)
 * - **Drought** - susza (-wilgotność, +nasłonecznienie)
 * - **Storm** - burza (+wilgotność, -nasłonecznienie)
 * 
 * #### 6. GUI (MainWindow)
 * - Interfejs użytkownika Qt
 * - Kontrolki do sterowania symulacją
 * - Slidery parametrów środowiska
 * - Przyciski wydarzeń klimatycznych
 * - Wyświetlanie planszy i statystyk
 * 
 * @section simulation_loop Pętla symulacji
 * 
 * Każdy tick symulacji składa się z następujących faz:
 * 
 * 1. **processClimateEvents()** - Aktualizacja wydarzeń klimatycznych
 * 2. **processEnergy()** - Metabolizm organizmów
 * 3. **processMovementPlanning()** - Planowanie ruchów
 * 4. **processMovementExecution()** - Wykonanie ruchów
 * 5. **processInteractions()** - Interakcje (polowanie)
 * 6. **processTileEffects()** - Efekty płytek
 * 7. **processReproduction()** - Rozmnażanie
 * 8. **cleanup()** - Usunięcie martwych
 * 
 * @section climate_system System klimatyczny
 * 
 * ### Parametry środowiska
 * 
 * Każda pozycja na mapie ma łączne parametry obliczane jako:
 * 
 * **Wynikowe = Globalne + Lokalne_płytki + Wydarzenia_klimatyczne**
 * 
 * - **Temperatura** - wpływa na metabolizm organizmów z preferencjami
 * - **Wilgotność** - (do przyszłego rozszerzenia)
 * - **Nasłonecznienie** - (do przyszłego rozszerzenia)
 * 
 * ### Wpływ na organizmy
 * 
 * - **Kryofile** - preferują temperaturę ≤ 10°C, metabolizm x0.7
 * - **Termofile** - preferują temperaturę ≥ 30°C, metabolizm x0.7
 * - **Domyślne** - normalne tempo metabolizmu
 * 
 * ### Wydarzenia klimatyczne
 * 
 * Wydarzenia mogą:
 * - Być wyzwalane automatycznie (z prawdopodobieństwem)
 * - Być wyzwalane ręcznie przez użytkownika
 * - Nakładać się (wiele aktywnych równocześnie)
 * - Mieć różną intensywność (1.0 = normalna, 2.0 = ekstremalna)
 * 
 * @section usage_sec Użycie
 * 
 * ### Kompilacja
 * 
 * ```bash
 * mkdir build && cd build
 * cmake ..
 * make
 * ```
 * 
 * ### Uruchomienie
 * 
 * ```bash
 * ./LifeLaboratory
 * ```
 * 
 * ### Konfiguracja
 * 
 * Edytuj `example_config.json` aby dostosować:
 * - Rozmiar mapy
 * - Wagi typów płytek
 * - Początkowe organizmy (pozycje, cechy)
 * 
 * @section extending_sec Rozszerzanie
 * 
 * ### Dodanie nowego typu organizmu
 * 
 * 1. Utwórz klasę dziedziczącą po `Organism`
 * 2. Zaimplementuj `reproduce()` do tworzenia potomków
 * 3. Opcjonalnie nadpisz `planMove()` dla własnego AI
 * 4. Dodaj do `SimulationBuilder` w metodzie parsowania JSON
 * 
 * ### Dodanie nowego typu płytki
 * 
 * 1. Utwórz klasę dziedziczącą po `Tile`
 * 2. Zaimplementuj `applyEffect(Organism*)` dla efektu
 * 3. Opcjonalnie nadpisz `isTraversable()` jeśli nieprzejezdna
 * 4. Dodaj do generacji mapy w `World::generateMap()`
 * 
 * ### Dodanie nowego typu wydarzenia
 * 
 * 1. Utwórz klasę dziedziczącą po `ClimateEvent`
 * 2. Ustaw modyfikatory w konstruktorze
 * 3. Opcjonalnie nadpisz `start()` dla dodatkowych efektów
 * 4. Dodaj do `ClimateEventManager::createRandomEvent()`
 * 5. Dodaj przycisk w `MainWindow` GUI
 * 
 * @section classes_overview Przegląd klas
 * 
 * ### Hierarchia dziedziczenia
 * 
 * ```
 * Entity (bazowa)
 * ├── Organism
 * │   ├── Producer
 * │   ├── Predator
 * │   ├── Omnivore
 * │   ├── Cryophile
 * │   └── Thermophile
 * └── Tile
 *     ├── NormalTile
 *     ├── AbundantTile
 *     ├── PoisonTile
 *     └── ImpassableTile
 * 
 * ClimateEvent
 * ├── Heatwave
 * ├── Freeze
 * ├── Drought
 * └── Storm
 * ```
 * 
 * @section author_sec Autor
 * 
 * Projekt stworzony jako symulacja ekologiczna w C++ z Qt.
 * 
 * @section license_sec Licencja
 * 
 * [Dodaj informacje o licencji]
 */

/**
 * @file entity.h
 * @brief Klasa bazowa Entity - podstawa dla wszystkich obiektów w symulacji
 */

/**
 * @class Entity
 * @brief Abstrakcyjna klasa bazowa dla wszystkich obiektów (organizmów i płytek)
 * 
 * Entity definiuje wspólny interfejs dla:
 * - Pozycjonowania (Position)
 * - Kolorowania (Color) do wizualizacji
 * 
 * Wszystkie obiekty w symulacji (Organism, Tile) dziedziczą po Entity.
 */

/**
 * @file organism.h  
 * @brief Klasa Organism - bazowa klasa dla wszystkich organizmów
 */

/**
 * @class Organism
 * @brief Reprezentuje żywy organizm w symulacji
 * 
 * Organism jest klasą bazową dla wszystkich żywych stworzeń.
 * Posiada właściwości:
 * - **Energia** - paliwo życia, spada co tick, wzrasta przez jedzenie
 * - **Rozmiar** - wpływa na koszty i możliwości
 * - **Prędkość** - jak daleko może się ruszyć
 * - **Punkty akcji (AP)** - limit działań na tick
 * - **Generacja** - numer pokolenia
 * - **Preferencja temperaturowa** - Kryofil/Termofil/Domyślny
 * - **Rodowód** - lineageId dla śledzenia linii
 * 
 * ### Cykl życia organizmu:
 * 1. **onTick()** - metabolizm, odejmowanie energii
 * 2. **planMove()** - AI planuje ruch
 * 3. **executeMovement()** - wykonanie ruchu
 * 4. **onInteract()** - interakcje z innymi
 * 5. Efekty płytek
 * 6. **canReproduce()** / **reproduce()** - rozmnażanie
 * 7. **die()** jeśli energia <= 0
 * 
 * ### Kluczowe metody:
 * - `climateMetabolismFactor()` - modyfikator metabolizmu od klimatu
 * - `movementApCost()` - koszt ruchu w AP
 * - `reproductionApCost()` - koszt reprodukcji w AP
 * - `reproductionEnergyCost()` - koszt reprodukcji w energii
 * - `findPlaceToBreed()` - szuka miejsca dla potomka
 */

/**
 * @file producer.h
 * @brief Klasa Producer - organizm produkujący (roślina)
 */

/**
 * @class Producer
 * @brief Producent - roślina czerpająca energię z otoczenia
 * 
 * Producer to podstawowy organizm produkujący energię z płytek.
 * - Czerpie energię z płytek (więcej na AbundantTile)
 * - Ucieka przed drapieżnikami
 * - Rozmnażanie: wymaga 150 energii, tworzy klon z mutacjami
 * - Mutacje: rozmiar ±1, prędkość ±1, maxEnergy ±20
 * 
 * ### AI producenta:
 * - Ucieka przed najbliższym drapieżnikiem (vision = 8)
 * - Jeśli nie ma zagrożenia, stoi w miejscu (oszczędza energię)
 */

/**
 * @file predator.h
 * @brief Klasa Predator - drapieżnik
 */

/**
 * @class Predator
 * @brief Drapieżnik polujący na inne organizmy
 * 
 * Predator to organizm myśliwski.
 * - Poluje na producent ów i mniejsze drapieżniki
 * - Nie poluje na swoich bezpośrednich krewnych
 * - Rozmnażanie: wymaga 300 energii
 * - Mutacje: jak Producer + vision ±1
 * 
 * ### AI drapieżnika:
 * - Goni najbliższą ofiarę w zasięgu wzroku
 * - Poluje gdy odległość ≤ 1.5
 * - Zabiera 60% energii ofiary, ofiara ginie
 * 
 * ### Polowanie:
 * - `attemptHunt(Organism* prey)` - próbuje zaatakować
 * - Sukces gdy prey jest żywa i w zasięgu
 * - Koszt: 1 AP
 */

/**
 * @file omnivore.h
 * @brief Klasa Omnivore - wszystkożerca
 */

/**
 * @class Omnivore
 * @brief Wszystkożerca - może jeść rośliny i zwierzęta
 * 
 * Omnivore łączy cechy Producer i Predator.
 * - Czerpie energię z płytek (jak Producer)
 * - Może polować (jak Predator, ale nie na drapieżników)
 * - Najbardziej wszechstronny, ale kosztowny
 * 
 * ### Rozmnażanie:
 * - Wymaga 250 energii
 * - Potomek ma szansę na mutację typu (może stać się Producer lub Predator)
 */

/**
 * @file cryophile.h / thermophile.h
 * @brief Specjalizacje temperaturowe
 */

/**
 * @class Cryophile
 * @brief Kryofil - organizm preferujący zimno
 * 
 * Cryophile to Producer z preferencją zimna.
 * - Optymalna temperatura: ≤ 10°C
 * - W zimnie: metabolizm x0.7 (70% zużycia energii)
 * - W cieple: normalny metabolizm
 */

/**
 * @class Thermophile
 * @brief Termofil - organizm preferujący ciepło
 * 
 * Thermophile to Producer z preferencją ciepła.
 * - Optymalna temperatura: ≥ 30°C
 * - W cieple: metabolizm x0.7 (70% zużycia energii)
 * - W zimnie: normalny metabolizm
 */
