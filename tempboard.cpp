// #include "tempboard.h"
// #include "organism.h"
// #include "tile.h"
// #include "predator.h"
// #include <QPainter>
// #include <QDebug>
// #include <QTimer>

// TempBoard::TempBoard(QWidget *parent)
//     : QWidget(parent), tileSize(20), lastTickTime(0), animationProgress(1.0)
// {
//     // Zapobiega migotaniu i ustawia bazowe tło
//     setAttribute(Qt::WA_OpaquePaintEvent);
//     animationTimer.start();
// }

// void TempBoard::setEntities(const std::vector<Entity*>& newEntities)
// {
//     entities = newEntities;
//     lastTickTime = animationTimer.elapsed();
//     animationProgress = 0.0;
//     this->update(); // Powiadom Qt, że trzeba wywołać paintEvent
// }

// void TempBoard::paintEvent(QPaintEvent *event)
// {
//     QPainter painter(this);
//     painter.setRenderHint(QPainter::Antialiasing);

//     // 1. CZYŚCIMY TŁO (żeby plansza nie "zniknęła" w czerni systemu)
//     painter.fillRect(rect(), QColor(30, 30, 30));

//     if (entities.empty()) {
//         painter.setPen(Qt::white);
//         painter.drawText(rect(), Qt::AlignCenter, "Oczekiwanie na dane z SimulationApp...");
//         return;
//     }

//     // Oblicz postęp animacji (0.0 = start, 1.0 = koniec)
//     qint64 currentTime = animationTimer.elapsed();
//     qint64 timeSinceLastTick = currentTime - lastTickTime;
//     qint64 animDuration = 200;
//     animationProgress = qMin(1.0, static_cast<double>(timeSinceLastTick) / animDuration);

//     // Jeśli animacja trwa, zaplanuj kolejne przerysowanie
//     if (animationProgress < 1.0) {
//         QTimer::singleShot(16, this, SLOT(update()));
//     }

//     // 2. RYSOWANIE KAFELKÓW (Logika kolegi)
//     for (Entity* entity : entities) {
//         if (!entity) continue;
//         if (Tile* tile = dynamic_cast<Tile*>(entity)) {
//             Color c = tile->getColor();
//             Position pos = tile->getPosition();
//             painter.setBrush(QColor(c.r, c.g, c.b));
//             painter.setPen(Qt::NoPen);
//             painter.drawRect(pos.x * tileSize, pos.y * tileSize, tileSize, tileSize);
//         }
//     }

//     // 3. RYSOWANIE ORGANIZMÓW Z INTERPOLACJĄ (Twoja logika)
//     for (Entity* entity : entities) {
//         if (!entity || dynamic_cast<Tile*>(entity)) continue;

//         if (Organism* org = dynamic_cast<Organism*>(entity)) {
//             if (!org->getIsAlive()) continue;

//             Position currentPos = org->getPosition();
//             Position prevPos = org->getPreviousPosition();
//             int currentSize = org->getSize();
//             Color c = org->getColor();

//             // Interpolacja pozycji dla płynnego ruchu
//             double drawX, drawY;
//             if (org->getIsMoving() && animationProgress < 1.0) {
//                 // Interpolacja liniowa między poprzednią a obecną pozycją
//                 drawX = prevPos.x + (currentPos.x - prevPos.x) * animationProgress;
//                 drawY = prevPos.y + (currentPos.y - prevPos.y) * animationProgress;
//             } else {
//                 // Organizm nie porusza się lub animacja zakończona
//                 drawX = currentPos.x;
//                 drawY = currentPos.y;
//             }

//             painter.setBrush(QColor(c.r, c.g, c.b));
//             painter.setPen(Qt::NoPen);

//             int offset = (tileSize - currentSize) / 2;
//             painter.drawEllipse(
//                 static_cast<int>(drawX * tileSize + offset),
//                 static_cast<int>(drawY * tileSize + offset),
//                 currentSize,
//                 currentSize
//                 );

//             if (dynamic_cast<Predator*>(org)) {
//                 painter.setPen(QPen(Qt::white, 1));
//                 painter.setBrush(Qt::NoBrush);
//                 painter.drawEllipse(
//                     static_cast<int>(drawX * tileSize + offset),
//                     static_cast<int>(drawY * tileSize + offset),
//                     currentSize,
//                     currentSize
//                     );
//             }
//         }
//     }
// }
// ALTERNATYWNA PROSTSZA WERSJA
// Zamiast timera po prostu liczymy klatki

// #include "tempboard.h"
// #include "organism.h"
// #include "tile.h"
// #include "predator.h"
// #include <QPainter>
// #include <QDebug>
// #include <QTimer>

// TempBoard::TempBoard(QWidget *parent)
//     : QWidget(parent), tileSize(20), animFrame(12), maxFrames(12)
// {
//     setAttribute(Qt::WA_OpaquePaintEvent);
// }

// void TempBoard::setEntities(const std::vector<Entity*>& newEntities)
// {
//     entities = newEntities;
//     animFrame = 0;  // Resetuj animację
//     this->update();
// }

// void TempBoard::paintEvent(QPaintEvent *event)
// {
//     QPainter painter(this);
//     painter.setRenderHint(QPainter::Antialiasing);

//     painter.fillRect(rect(), QColor(30, 30, 30));

//     if (entities.empty()) {
//         painter.setPen(Qt::white);
//         painter.drawText(rect(), Qt::AlignCenter, "Oczekiwanie na dane z SimulationApp...");
//         return;
//     }

//     // Oblicz progress jako procent wykonanej animacji
//     double progress = (double)animFrame / (double)maxFrames;
//     if (progress > 1.0) progress = 1.0;

//     // Jeśli animacja nie skończona, zaplanuj następną klatkę
//     if (animFrame < maxFrames) {
//         animFrame++;
//         QTimer::singleShot(16, this, SLOT(update()));
//     }

//     // Rysuj kafelki
//     for (Entity* entity : entities) {
//         if (!entity) continue;
//         if (Tile* tile = dynamic_cast<Tile*>(entity)) {
//             Color c = tile->getColor();
//             Position pos = tile->getPosition();
//             painter.setBrush(QColor(c.r, c.g, c.b));
//             painter.setPen(Qt::NoPen);
//             painter.drawRect(pos.x * tileSize, pos.y * tileSize, tileSize, tileSize);
//         }
//     }

//     // Rysuj organizmy z animacją
//     for (Entity* entity : entities) {
//         if (!entity || dynamic_cast<Tile*>(entity)) continue;

//         if (Organism* org = dynamic_cast<Organism*>(entity)) {
//             if (!org->getIsAlive()) continue;

//             Position curr = org->getPosition();
//             Position prev = org->getPreviousPosition();
//             int sz = org->getSize();
//             Color c = org->getColor();

//             double x, y;
//             if (org->getIsMoving() && progress < 1.0) {
//                 // Płynne przejście między pozycjami
//                 x = prev.x + (curr.x - prev.x) * progress;
//                 y = prev.y + (curr.y - prev.y) * progress;
//             } else {
//                 x = curr.x;
//                 y = curr.y;
//             }

//             painter.setBrush(QColor(c.r, c.g, c.b));
//             painter.setPen(Qt::NoPen);

//             int off = (tileSize - sz) / 2;
//             painter.drawEllipse(
//                 (int)(x * tileSize + off),
//                 (int)(y * tileSize + off),
//                 sz, sz
//                 );

//             // Ramka dla drapieżników
//             if (dynamic_cast<Predator*>(org)) {
//                 painter.setPen(QPen(Qt::white, 1));
//                 painter.setBrush(Qt::NoBrush);
//                 painter.drawEllipse(
//                     (int)(x * tileSize + off),
//                     (int)(y * tileSize + off),
//                     sz, sz
//                     );
//             }
//         }
//     }
// }
#include "tempboard.h"
#include "organism.h"
#include "tile.h"
#include "predator.h"
#include <QPainter>
#include <QTimer>

TempBoard::TempBoard(QWidget *parent)
    : QWidget(parent), tileSize(20), step(12), maxStep(12)
{
    setAttribute(Qt::WA_OpaquePaintEvent);
    setAttribute(Qt::WA_NoSystemBackground);
}

void TempBoard::setEntities(const std::vector<Entity*>& newEntities)
{
    entities = newEntities;
    step = 0;
    update();
}

void TempBoard::paintEvent(QPaintEvent *event)
{
    QPainter p(this);
    p.fillRect(rect(), QColor(30, 30, 30));

    if (entities.empty()) {
        p.setPen(Qt::white);
        p.drawText(rect(), Qt::AlignCenter, "Oczekiwanie na dane...");
        return;
    }

    double t = (double)step / maxStep;
    if (t > 1.0) t = 1.0;

    // ease out - bakteria zwalnia pod koniec
    t = 1.0 - (1.0 - t) * (1.0 - t);

    if (step < maxStep) {
        step++;
        QTimer::singleShot(16, this, SLOT(update()));
    }

    // tiles
    for (Entity* e : entities) {
        if (!e) continue;
        if (Tile* tile = dynamic_cast<Tile*>(e)) {
            Color c = tile->getColor();
            Position pos = tile->getPosition();
            p.fillRect(pos.x * tileSize, pos.y * tileSize, tileSize, tileSize, QColor(c.r, c.g, c.b));
        }
    }

    // organisms
    p.setPen(Qt::NoPen);
    for (Entity* e : entities) {
        if (!e || dynamic_cast<Tile*>(e)) continue;

        if (Organism* org = dynamic_cast<Organism*>(e)) {
            if (!org->getIsAlive()) continue;

            Position curr = org->getPosition();
            Position prev = org->getPreviousPosition();

            double x = curr.x, y = curr.y;
            if (org->getIsMoving() && t < 1.0) {
                x = prev.x + (curr.x - prev.x) * t;
                y = prev.y + (curr.y - prev.y) * t;
            }

            Color c = org->getColor();
            int sz = org->getSize()*4;
            int off = (tileSize - sz) / 2;

            p.setBrush(QColor(c.r, c.g, c.b));
            p.drawEllipse((int)(x * tileSize + off), (int)(y * tileSize + off), sz, sz);

            if (dynamic_cast<Predator*>(org)) {
                p.setBrush(Qt::NoBrush);
                p.setPen(QPen(Qt::white, 1));
                p.drawEllipse((int)(x * tileSize + off), (int)(y * tileSize + off), sz, sz);
                p.setPen(Qt::NoPen);
            }
        }
    }
}
