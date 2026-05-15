# Life Laboratory - Symulator Ewolucji

## Opis projektu

Life Laboratory to symulator ewolucji napisany w C++ z użyciem Qt. Projekt symuluje życie różnych organizmów na planszy z różnymi typami środowisk (płytek).

## Główne założenia

### Cel
Pokazanie uproszczonej symulacji ewolucji, gdzie:
- Bakterie ewoluują poprzez mutacje podczas rozmnażania
- Drapieżniki polują na producencie
- Organizmy muszą zarządzać energią aby przetrwać
- Środowisko (płytki) wpływa na organizmy

### Gatunki organizmów

#### Producer (Producent) 🟢
- **Kolor:** Zielony
- **Energia:** Zyskuje energię przez fotosyntezę (+5 na turę)
- **Koszt życia:** -1 energia na turę
- **Ruch:** Losowy
- **Rozmnażanie:** Przy 80% maksymalnej energii
- **Ewolucja:** Losowe mutacje (20% szansa) wpływające na energię, rozmiar, szybkość lub temperaturę

#### Predator (Drapieżnik) 🔴
- **Kolor:** Czerwony
- **Energia:** Zyskuje energię polując na inne organizmy
- **Koszt życia:** -3 energia na turę (wyższy niż producenci)
- **Ruch:** Śledzi najbliższą ofiarę w zasięgu wzroku
- **Polowanie:** Zabiera połowę energii ofiary
- **Rozmnażanie:** Przy energii ≥ 150
- **Ewolucja:** Mutacje wpływające na energię, rozmiar, szybkość, wzrok

#### Thermophile (Termofil) 🔴🟠
- **Kolor:** Pomarańczowo-czerwony
- **Preferowana temperatura:** 40°C
- **Zachowanie:** Szuka najcieplejszych miejsc
- **Bonus:** +5 energii gdy temperatura różni się ≤10°C od preferowanej
- **Kara:** -0.2 × różnica temperatury gdy jest za zimno/gorąco
- **Ewolucja:** Może mutować w stronę jeszcze wyższych temperatur

#### Cryophile (Kriofil) 🔵💙
- **Kolor:** Niebieski
- **Preferowana temperatura:** -5°C
- **Zachowanie:** Szuka najzimniejszych miejsc
- **Bonus:** +5 energii gdy temperatura różni się ≤10°C od preferowanej
- **Kara:** -0.2 × różnica temperatury gdy jest za zimno/gorąco
- **Ewolucja:** Może mutować w stronę jeszcze niższych temperatur

### Typy płytek

#### NormalTile (Normalna płytka) ⬜
- Standardowa płytka, bez specjalnych efektów
- **Kolor:** Szary/biały

#### AbundantTile (Płytka obfita) 🟩
- Daje dodatkową energię producentom
- **Kolor:** Jasny zielony
- **Efekt:** +2 energia dla producentów

#### PoisonTile (Trująca płytka) 🟪
- Zabiera energię organizmom
- **Kolor:** Fioletowy
- **Efekt:** -3 energia wszystkim organizmom

#### ImpassableTile (Nieprzejezdna płytka) ⬛
- Blokuje ruch organizmów
- **Kolor:** Czarny
- **Przejezdność:** NIE

## System ewolucji

### Mutacje (20% szansa przy reprodukcji)

Każdy organizm podczas rozmnażania ma 20% szansę na mutację. Mutacje są losowe i mogą wpływać na:

1. **Maksymalną energię** - organizm może przechować więcej energii
2. **Rozmiar** - wpływa na widoczność i kolizje
3. **Szybkość** - więcej punktów akcji = więcej ruchów
4. **Specjalne cechy:**
   - Drapieżniki: zasięg wzroku
   - Producenci/Termofile/Kriofile: preferowana temperatura

### Wizualna reprezentacja mutacji

Każda mutacja zmienia kolor organizmu, dzięki czemu można zobaczyć ewolucję:
- Czerwony: wyższa energia, wyższa temperatura
- Zielony: większy rozmiar
- Niebieski: większa szybkość, niższa temperatura, lepszy wzrok

## Struktura projektu

```
LifeLabolatory/
├── entity.h/cpp          # Bazowa klasa Entity
├── organism.h/cpp        # Klasa bazowa organizmów
├── producer.h/cpp        # Producent
├── predator.h/cpp        # Drapieżnik
├── thermophile.h/cpp     # Termofil
├── cryophile.h/cpp       # Kriofil
├── tile.h/cpp            # Klasa bazowa płytek
├── normaltile.h/cpp      # Normalna płytka
├── abundanttile.h/cpp    # Płytka obfita
├── poisontile.h/cpp      # Trująca płytka
├── impassabletile.h/cpp  # Nieprzejezdna płytka
├── world.h/cpp           # Świat gry
├── simengine.h/cpp       # Silnik symulacji
├── simulationapp.h/cpp   # Aplikacja symulacji
├── simulationbuilder.h/cpp # Builder (czyta JSON)
├── statmanager.h/cpp     # Statystyki
├── mainwindow.h/cpp      # Główne okno (GUI)
├── example_config.json   # Przykładowa konfiguracja
├── improved_config.json  # Ulepszona konfiguracja
└── ZMIANY.md            # Dokumentacja zmian
```

## Kompilacja i uruchomienie

### Wymagania
- C++17 lub nowszy
- Qt 5.15+ lub Qt 6.x
- CMake 3.16+

### Kompilacja z CMake

```bash
cd LifeLabolatory
mkdir build
cd build
cmake ..
make
./LifeLabolatory
```

### Kompilacja w Qt Creator

1. Otwórz `CMakeLists.txt` w Qt Creator
2. Skonfiguruj projekt (wybierz kit)
3. Uruchom projekt (Ctrl+R)

## Konfiguracja (JSON)

Program czyta konfigurację z pliku `example_config.json` lub `improved_config.json`:

```json
{
  "world": {
    "width": 80,
    "height": 60,
    "mapConfig": {
      "baseNormalWeight": 100,
      "baseAbundantWeight": 12,
      "basePoisonWeight": 4,
      "baseImpassableWeight": 5,
      "clusterBonus": 150
    }
  },
  "organisms": [
    {
      "type": "producer",
      "x": 10, "y": 10,
      "startEnergy": 80.0,
      "maxEnergy": 200.0,
      "size": 1,
      "speed": 1,
      "maxAP": 1,
      "preferredTemperature": 20.0
    }
  ]
}
```

### Parametry świata

- **width, height** - rozmiar mapy
- **mapConfig** - wagi generowania różnych typów płytek
- **clusterBonus** - bonus do tworzenia "biomów" (skupisk podobnych płytek)

### Parametry organizmów

- **type** - "producer", "predator", "thermophile", "cryophile"
- **x, y** - pozycja startowa
- **startEnergy** - energia początkowa
- **maxEnergy** - maksymalna energia
- **size** - rozmiar (wpływa na widoczność)
- **speed** - szybkość (punkty akcji)
- **maxAP** - maksymalne punkty akcji
- **vision** - zasięg wzroku (tylko drapieżniki)
- **preferredTemperature** - preferowana temperatura (tylko producenci)

## Obsługa GUI

### Przyciski

- **Start** - rozpoczyna ciągłą symulację
- **Stop/Pause** - zatrzymuje symulację
- **Step** - wykonuje jeden krok (dla debugowania)

### Obserwacja

- Organizmy są rysowane jako kolorowe kwadraty
- Płytki są rysowane jako tło
- Kolor organizmu zmienia się z mutacjami

## Ciekawe obserwacje

Po uruchomieniu symulacji można zaobserwować:

1. **Cykle drapieżnik-ofiara** - populacje rosną i maleją cyklicznie
2. **Ewolucję kolorów** - organizmy stopniowo zmieniają kolory
3. **Wyginięcie** - słabo przystosowane organizmy wymierają
4. **Dominację** - najlepiej przystosowane organizmy dominują
5. **Segregację przestrzenną** - termofile i kriofile migrują do odpowiednich obszarów

## Historia projektu

Projekt został stworzony wspólnie przez dwóch studentów. Powstały dwie wersje z różnymi pomysłami na implementację, które zostały następnie połączone i naprawione.

### Główne problemy w oryginalnej wersji:
- Podwójne wywoływanie `onTick()` powodowało zbyt szybką utratę energii
- Brak fotosyntezy dla producentów
- Mutacje były zbyt rzadkie (tylko co 10 pokoleń)
- Brak walidacji ruchu (organizmy wychodziły poza mapę)
- Brak wsparcia dla wszystkich typów organizmów w JSON

Wszystkie te problemy zostały naprawione. Szczegóły w pliku `ZMIANY.md`.

## Licencja

Projekt edukacyjny. Możesz go modyfikować i rozwijać według własnych potrzeb.

## Autorzy

- [Imię 1] - początkowa wersja
- [Imię 2] - druga wersja
- Claude (AI) - połączenie i naprawa wersji

## Dalszy rozwój

Pomysły na rozszerzenie projektu:
- System genów zamiast prostych parametrów
- Wykresy populacji w czasie rzeczywistym
- Więcej typów interakcji (symbioza, pasożytnictwo)
- Zapisywanie/wczytywanie stanu symulacji
- Cykle dnia/nocy wpływające na środowisko
- Więcej typów płytek (woda, góry, jaskinie)
