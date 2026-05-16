// ALTERNATYWNA PROSTSZA WERSJA
// Zamiast timera po prostu liczymy klatki

#include "tempboard.h"
#include "organism.h"
#include "tile.h"
#include "predator.h"
#include <QPainter>
#include <QDebug>
#include <QTimer>

TempBoard::TempBoard(QWidget *parent)
    : QWidget(parent), tileSize(20), animFrame(12), maxFrames(12)
{
    setAttribute(Qt::WA_OpaquePaintEvent);
}

void TempBoard::setEntities(const std::vector<Entity*>& newEntities)
{
    entities = newEntities;
    animFrame = 0;  // Resetuj animację
    this->update();
}

void TempBoard::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    painter.fillRect(rect(), QColor(30, 30, 30));

    if (entities.empty()) {
        painter.setPen(Qt::white);
        painter.drawText(rect(), Qt::AlignCenter, "Oczekiwanie na dane z SimulationApp...");
        return;
    }

    // Oblicz progress jako procent wykonanej animacji
    double progress = (double)animFrame / (double)maxFrames;
    if (progress > 1.0) progress = 1.0;

    // Jeśli animacja nie skończona, zaplanuj następną klatkę
    if (animFrame < maxFrames) {
        animFrame++;
        QTimer::singleShot(16, this, SLOT(update()));
    }

    // Rysuj kafelki
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

    // Rysuj organizmy z animacją
    for (Entity* entity : entities) {
        if (!entity || dynamic_cast<Tile*>(entity)) continue;

        if (Organism* org = dynamic_cast<Organism*>(entity)) {
            if (!org->getIsAlive()) continue;

            Position curr = org->getPosition();
            Position prev = org->getPreviousPosition();
            int sz = org->getSize();
            Color c = org->getColor();

            double x, y;
            if (org->getIsMoving() && progress < 1.0) {
                // Płynne przejście między pozycjami
                x = prev.x + (curr.x - prev.x) * progress;
                y = prev.y + (curr.y - prev.y) * progress;
            } else {
                x = curr.x;
                y = curr.y;
            }

            painter.setBrush(QColor(c.r, c.g, c.b));
            painter.setPen(Qt::NoPen);

            int off = (tileSize - sz) / 2;
            painter.drawEllipse(
                (int)(x * tileSize + off),
                (int)(y * tileSize + off),
                sz, sz
            );

            // Ramka dla drapieżników
            if (dynamic_cast<Predator*>(org)) {
                painter.setPen(QPen(Qt::white, 1));
                painter.setBrush(Qt::NoBrush);
                painter.drawEllipse(
                    (int)(x * tileSize + off),
                    (int)(y * tileSize + off),
                    sz, sz
                );
            }
        }
    }
}
