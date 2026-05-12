#include "tempboard.h"
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

    for (Entity* entity : entities) {
        if (entity != nullptr) {
            Color logicColor = entity->getColor();
            QColor qtColor(logicColor.r, logicColor.g, logicColor.b);

            painter.setBrush(qtColor);
            painter.setPen(Qt::NoPen);

            Position pos = entity->getPosition();
            painter.drawEllipse(pos.x * tileSize, pos.y * tileSize, tileSize, tileSize);
        }
    }
}