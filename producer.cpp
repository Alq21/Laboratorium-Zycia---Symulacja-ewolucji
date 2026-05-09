#include "producer.h"


Producer::Producer(Position pos, Color col, double energy, int size, int speed, int maxAP, double prefTemp)
    : Organism(pos, col, energy, size, speed, maxAP), _preferredTemperature(prefTemp) {
}

void Producer::photosynthesis(Tile* currentTile) {
    // Tutaj w przyszłości sprawdzanie, czy stoimy na dobrym kafelku i dodaje sobie energii

}
