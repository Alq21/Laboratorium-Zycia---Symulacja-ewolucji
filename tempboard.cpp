#include "tempboard.h"
#include "tile.h"
#include <QPainter>

TempBoard::TempBoard(QWidget *parent) : QWidget(parent), tileSize(10) {
    setMinimumSize(400, 400);
}

void TempBoard::setEntities(const std::vector<Entity*>& newEntities) {
    entities = newEntities;
    update();
}

void TempBoard::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.fillRect(rect(), QColor(30, 30, 30));
    painter.setPen(Qt::NoPen);

    for (Entity* entity : entities) {
        if (!entity || !dynamic_cast<Tile*>(entity)) continue;
        Color c = entity->getColor();
        Position pos = entity->getPosition();
        painter.setBrush(QColor(c.r, c.g, c.b));
        painter.drawRect(pos.x * tileSize, pos.y * tileSize, tileSize, tileSize);
    }

    for (Entity* entity : entities) {
        if (!entity || dynamic_cast<Tile*>(entity)) continue;
        Color c = entity->getColor();
        Position pos = entity->getPosition();
        painter.setBrush(QColor(c.r, c.g, c.b));
        painter.drawEllipse(pos.x * tileSize + 1, pos.y * tileSize + 1,
                            tileSize - 2, tileSize - 2);
    }
}
