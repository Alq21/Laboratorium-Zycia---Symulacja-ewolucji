
#include "tempboard.h"
#include "abundanttile.h"
#include "impassabletile.h"
#include "normaltile.h"
#include "organism.h"
#include "poisontile.h"
#include "tile.h"
#include "predator.h"
#include <QPainter>
TempBoard::TempBoard(QWidget *parent)
    : QWidget(parent),
    tileSize(20),
    step(0), maxStep(16),
    t(1.0)   // zaczyna "gotowy" — brak animacji przed pierwszym setEntities
{
    setAttribute(Qt::WA_OpaquePaintEvent);
    setAttribute(Qt::WA_NoSystemBackground);
    animTimer = new QTimer(this);
    animTimer->setInterval(19);   // ~60 FPS
    connect(animTimer, &QTimer::timeout, this, &TempBoard::animationTick);
}
void TempBoard::setEntities(const std::vector<Entity*>& newEntities)
{
    entities = newEntities;

    step=0;
    t=0.0;
    if (!animTimer->isActive()) {

        animTimer->start();
    }

}
void TempBoard::animationTick()
{
    step++;

    double raw = (double)step / maxStep;
    if (raw >= 1.0) {
        raw = 1.0;
        animTimer->stop();   // animacja
    }
    t = 1.0 - (1.0 - raw) * (1.0 - raw);
    update();
}
void TempBoard::paintEvent(QPaintEvent* )
{
    QPainter p(this);

    p.fillRect(rect(), QColor(30, 30, 30));
    if (entities.empty()) {
        p.setPen(Qt::white);
        p.drawText(rect(), Qt::AlignCenter, "Oczekiwanie na dane...");
        return;
    }
    // 1. plytki
    for (Entity* e : entities) {
        if (Tile* tile = dynamic_cast<Tile*>(e)) {
            Position pos = tile->getPosition();
            int x = pos.x * tileSize;
            int y = pos.y * tileSize;


            if (dynamic_cast<NormalTile*>(tile)) {

                p.fillRect(x, y, tileSize, tileSize, QColor(70, 70, 70));
                p.setPen(QPen(QColor(50, 50, 50)));
                p.drawRect(x, y, tileSize, tileSize);
            }
            else if (dynamic_cast<AbundantTile*>(tile)) {

                p.fillRect(x, y, tileSize, tileSize, QColor(100, 180, 100));
            }
            else if (dynamic_cast<PoisonTile*>(tile)) {

                p.fillRect(x, y, tileSize, tileSize, QColor(180, 80, 80));
            }
            else if (dynamic_cast<ImpassableTile*>(tile)) {

                p.fillRect(x, y, tileSize, tileSize, QColor(30, 30, 30));
            }
        }
    }
    // 2. Organizmy
    p.setPen(Qt::NoPen);
    for (Entity* e : entities) {
        if (!e) continue;
        Organism* org = dynamic_cast<Organism*>(e);
        if (!org || !org->getIsAlive()) continue;
        Position curr = org->getPosition();
        Position prev = org->getPreviousPosition();
        double drawX = curr.x;
        double drawY = curr.y;
        // Interpoluj tylko jeśli faktycznie się ruszył I animacja trwa
        if (org->getIsMoving()){
            drawX = prev.x + (curr.x - prev.x) * t;
            drawY = prev.y + (curr.y - prev.y) * t;
        }
        Color c  = org->getColor();
        int   sz = std::max(4, org->getSize() * 2);
        int   off = (tileSize - sz) / 2;
        int   px  = static_cast<int>(drawX * tileSize) + off;
        int   py  = static_cast<int>(drawY * tileSize) + off;
        p.setBrush(QColor(c.r, c.g, c.b));
        p.drawEllipse(px, py, sz, sz);
        // Biała obwódka dla drapieżników
        if (dynamic_cast<Predator*>(org)) {
            p.setBrush(Qt::NoBrush);
            p.setPen(QPen(Qt::white, 1));
            p.drawEllipse(px, py, sz, sz);
            p.setPen(Qt::NoPen);
            p.setBrush(QColor(c.r, c.g, c.b));  // przywróć dla następnego
        }
    }
}