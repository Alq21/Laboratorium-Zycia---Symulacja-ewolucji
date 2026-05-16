#include "tempboard.h"
#include "simulationapp.h"
#include "organism.h"
#include "predator.h"
#include "producer.h"
#include "tile.h"
#include "normaltile.h"
#include "abundanttile.h"
#include "poisontile.h"
#include "impassabletile.h"
#include <QPainter>
#include <QTimer>

TempBoard::TempBoard(QWidget *parent)
    : QWidget(parent)
    , tileSize(20)
{

}

void TempBoard::setEntities(const std::vector<Entity*>& newEntities)
{
    entities = newEntities;
    update();
}

void TempBoard::paintEvent(QPaintEvent*)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    p.fillRect(rect(), QColor(30, 30, 30));

    if (entities.empty()) {
        p.setPen(Qt::white);
        p.drawText(rect(), Qt::AlignCenter, "Oczekiwanie na dane...");
        return;
    }

    // PŁYTKI
    for (Entity* e : entities) {
        if (!e) continue;
        if (Tile* tile = dynamic_cast<Tile*>(e)) {
            Position pos = tile->getPosition();
            int x = pos.x * tileSize;
            int y = pos.y * tileSize;

            if (dynamic_cast<NormalTile*>(tile)) {
                p.fillRect(x, y, tileSize, tileSize, QColor(70, 70, 70));
                p.setPen(QPen(QColor(50, 50, 50), 0.5));
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

    // organizmy + interpolacja

    // Pobiera interpolację z SimulationApp (0.0 do 1.0)
    double t = simApp ? simApp->getInterpolation() : 1.0;

    for (Entity* e : entities) {
        Organism* org = dynamic_cast<Organism*>(e);
        if (!org || !org->getIsAlive()) continue;

        Position curr = org->getPosition();
        Position last = org->getLastPosition();

        // Interpolacja: last + (curr - last) * t
        double drawX = last.x + (curr.x - last.x) * t;
        double drawY = last.y + (curr.y - last.y) * t;

        Color c = org->getColor();
         int baseSize = org->getSize();
        if (baseSize < 1) baseSize = 1;
        if (baseSize > 10) baseSize = 10;

        int sz = 6 + (baseSize * 2);


        int px = (int)(drawX * tileSize);
        int py = (int)(drawY * tileSize);

        p.setBrush(QColor(0, 0, 0, 80));
        p.setPen(Qt::NoPen);
        p.drawEllipse(px + 2, py + 2, sz, sz);

        p.setBrush(QColor(c.r, c.g, c.b));

        p.drawEllipse(px, py, sz, sz);


        if (dynamic_cast<Predator*>(org)) {
            p.setPen(QPen(Qt::white, 2));
            p.setBrush(Qt::NoBrush);
            p.drawEllipse(px + 1, py + 1, sz - 2, sz - 2);
        }
    }


    if (simApp && t < 1.0) {
        QTimer::singleShot(16, this, QOverload<>::of(&QWidget::update));
    }
}