#include "tempboard.h"
#include "organism.h"
#include "predator.h"
#include "producer.h"
#include "tile.h"
#include <QPainter>
#include <iostream>

TempBoard::TempBoard(QWidget *parent)
    : QWidget{parent}, tileSize(20)
{
}

void TempBoard::setEntities(const std::vector<Entity*>& newEntities) {
    entities = newEntities;
    this->update();
}

void TempBoard::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    std::cout << "\n=== PAINT EVENT CALLED ===" << std::endl;
    std::cout << "Total entities in vector: " << entities.size() << std::endl;

    // 1. Czyść tło
    painter.fillRect(rect(), QColor(30, 30, 30));

    if (entities.empty()) {
        painter.setPen(Qt::white);
        painter.drawText(rect(), Qt::AlignCenter, "Oczekiwanie na dane...");
        return;
    }

    // 2. RYSUJ PŁYTKI
    for (Entity* entity : entities) {
        if (!entity) continue;

        if (Tile* tile = dynamic_cast<Tile*>(entity)) {
            Color c = tile->getColor();
            Position pos = tile->getPosition();
            painter.setBrush(QColor(c.r, c.g, c.b));
            painter.setPen(Qt::NoPen);
            painter.drawRect(pos.x * tileSize, pos.y * tileSize, tileSize, tileSize);
        }
    }
    int organismCount = 0;

    for (Entity* entity : entities) {
        if (!entity) continue;
        if (dynamic_cast<Tile*>(entity)) continue;

        Organism* org = dynamic_cast<Organism*>(entity);
        if (!org || !org->getIsAlive()) continue;

        organismCount++;
        Position pos = org->getPosition();
        Color c = org->getColor();

        std::cout << "Drawing #" << organismCount << " at (" << pos.x << "," << pos.y
                  << ") Color(" << c.r << "," << c.g << "," << c.b
                  << ") Size:" << org->getSize() << std::endl;

        // Średni rozmiar (nie za duży, nie za mały)
        int visualSize = org->getSize();

        int centerX = pos.x * tileSize;
        int centerY = pos.y * tileSize;

        // Rysuj WYRAŹNE kółko
        painter.setBrush(QColor(c.r, c.g, c.b));

        painter.drawEllipse(centerX, centerY, visualSize, visualSize);


    }

    std::cout << "DRAWN: " << organismCount << std::endl;
}
    // 3. RYSUJ ORGANIZMY - połączone z liczeniem
    // int organismCount = 0;

    // for (Entity* entity : entities) {
    //     if (!entity) continue;

    //     // Pomijamy płytki
    //     if (dynamic_cast<Tile*>(entity)) continue;

    //     // Sprawdzamy czy to organizm
    //     Organism* org = dynamic_cast<Organism*>(entity);
    //     if (!org) continue;

    //     // Sprawdzamy czy żyje
    //     if (!org->getIsAlive()) {
    //         std::cout << "Skipping DEAD organism at ("
    //                   << org->getPosition().x << "," << org->getPosition().y << ")" << std::endl;
    //         continue;
    //     }

    //     // === ORGANIZM ŻYWY - RYSUJEMY! ===
    //     organismCount++;

    //     Position pos = org->getPosition();
    //     Color c = org->getColor();

    //     std::cout << "Drawing organism #" << organismCount << " at ("
    //               << pos.x << "," << pos.y << ") Color: ("
    //               << c.r << "," << c.g << "," << c.b << ")" << std::endl;

    //     // Powiększony rozmiar dla lepszej widoczności
    //     int visualSize = org->getSize() * 4;
    //     if (visualSize > tileSize - 2) visualSize = tileSize - 2;
    //     if (visualSize < 8) visualSize = 8;  // Minimum 8 pikseli

    //     int offset = (tileSize - visualSize) / 2;
    //     int centerX = pos.x * tileSize + offset;
    //     int centerY = pos.y * tileSize + offset;

    //     // Rysuj kolorowe kółko
    //     painter.setBrush(QColor(c.r, c.g, c.b));
    //     painter.setPen(Qt::NoPen);
    //     painter.drawEllipse(centerX, centerY, visualSize, visualSize);

    //     // CZARNA obwódka dla WSZYSTKICH
    //     painter.setPen(QPen(Qt::black, 3));
    //     painter.setBrush(Qt::NoBrush);
    //     painter.drawEllipse(centerX, centerY, visualSize, visualSize);

    //     // Biała obwódka dla predatorów
    //     if (dynamic_cast<Predator*>(org)) {
    //         painter.setPen(QPen(Qt::white, 2));
    //         painter.drawEllipse(centerX+2, centerY+2, visualSize-4, visualSize-4);
    //     }

    /*

    std::cout << "=== TOTAL ORGANISMS DRAWN: " << organismCount << " ===" << std::endl;
}*/
