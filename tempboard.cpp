#include "tempboard.h"
#include "organism.h"
#include "tile.h"
#include "predator.h"
#include <QPainter>
#include <QDebug>

TempBoard::TempBoard(QWidget *parent)
    : QWidget(parent), tileSize(20)
{
    // Zapobiega migotaniu i ustawia bazowe tło
    setAttribute(Qt::WA_OpaquePaintEvent);
}

void TempBoard::setEntities(const std::vector<Entity*>& newEntities)
{
    entities = newEntities;
    this->update(); // Powiadom Qt, że trzeba wywołać paintEvent
}

void TempBoard::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // 1. CZYŚCIMY TŁO (żeby plansza nie "zniknęła" w czerni systemu)
    painter.fillRect(rect(), QColor(30, 30, 30));

    if (entities.empty()) {
        painter.setPen(Qt::white);
        painter.drawText(rect(), Qt::AlignCenter, "Oczekiwanie na dane z SimulationApp...");
        return;
    }

    // 2. RYSOWANIE KAFELKÓW (Logika kolegi)
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

    // 3. RYSOWANIE ORGANIZMÓW (Twoja logika)
    for (Entity* entity : entities) {
        if (!entity || dynamic_cast<Tile*>(entity)) continue;

        if (Organism* org = dynamic_cast<Organism*>(entity)) {
            if (!org->getIsAlive()) continue;

            Position pos = org->getPosition();
            int currentSize = org->getSize();
            Color c = org->getColor();

            painter.setBrush(QColor(c.r, c.g, c.b));
            painter.setPen(Qt::NoPen);

            int offset = (tileSize - currentSize) / 2;
            painter.drawEllipse(
                pos.x * tileSize + offset,
                pos.y * tileSize + offset,
                currentSize,
                currentSize
                );

            if (dynamic_cast<Predator*>(org)) {
                painter.setPen(QPen(Qt::white, 1));
                painter.setBrush(Qt::NoBrush);
                painter.drawEllipse(
                    pos.x * tileSize + offset,
                    pos.y * tileSize + offset,
                    currentSize,
                    currentSize
                    );
            }
        }
    }
}