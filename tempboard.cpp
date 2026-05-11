#include "TempBoard.h"
#include <QPainter>

TempBoard::TempBoard(QWidget *parent) : QWidget(parent) {

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

            QColor logicColor = entity->getColor();


           QColor qtColor(logicColor.red(), logicColor.green(), logicColor.blue());

            painter.setBrush(qtColor);
            painter.setPen(Qt::NoPen);

            Position pos = entity->getPosition();
            painter.drawEllipse(pos.x * tileSize, pos.y * tileSize, tileSize, tileSize);
        }
        }
    }
