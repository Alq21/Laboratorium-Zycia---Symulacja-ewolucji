#include "tempboard.h"
#include "abundanttile.h"
#include "impassabletile.h"
#include "normaltile.h"
#include "organism.h"
#include "poisontile.h"
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
    std::cout << "Total entities in vector: " << entities.size() << std::endl;

    // 1. Czyść tło
    painter.fillRect(rect(), QColor(60, 60, 60));

    if (entities.empty()) {
        painter.setPen(Qt::white);
        painter.drawText(rect(), Qt::AlignCenter, "Oczekiwanie na dane...");
        return;
    }

    // 2. RYSUJ PŁYTKI
    for (Entity* entity : entities) {
        if (!entity) continue;

        if (Tile* tile = dynamic_cast<Tile*>(entity)) {
            Position pos = tile->getPosition();
            int x = pos.x * tileSize;
            int y = pos.y * tileSize;

            if (dynamic_cast<NormalTile*>(tile)) {
                painter.fillRect(x, y, tileSize, tileSize, QColor(70, 70, 70));
                painter.setPen(QPen(QColor(50, 50, 50), 0.5));
                painter.drawRect(x, y, tileSize, tileSize);
            }
            else if (dynamic_cast<AbundantTile*>(tile)) {
                painter.fillRect(x, y, tileSize, tileSize, QColor(100, 180, 100));
            }
            else if (dynamic_cast<PoisonTile*>(tile)) {
                painter.fillRect(x, y, tileSize, tileSize, QColor(180, 80, 80));
            }
            else if (dynamic_cast<ImpassableTile*>(tile)) {
                painter.fillRect(x, y, tileSize, tileSize, QColor(30, 30, 30));
            }
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
                  << ") Size:" << org->getSize()*3 << std::endl;

        // Średni rozmiar (nie za duży, nie za mały)
        int visualSize = org->getSize()*2;

        int centerX = pos.x * tileSize;
        int centerY = pos.y * tileSize;

        // Rysuj WYRAŹNE kółko
        painter.setBrush(QColor(c.r, c.g, c.b));

        painter.drawEllipse(centerX, centerY, visualSize, visualSize);


    }

    std::cout << "DRAWN: " << organismCount << std::endl;
}
