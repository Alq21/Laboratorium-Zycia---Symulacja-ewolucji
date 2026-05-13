#include "tempboard.h"
#include "organism.h"
#include <QPainter>
#include <QBrush>
#include <QPen>

TempBoard::TempBoard(QWidget *parent) : QWidget(parent), tileSize(20)
{

    setAutoFillBackground(true);
    QPalette pal = palette();
    pal.setColor(QPalette::Window, Qt::black);
    setPalette(pal);
}

void TempBoard::setEntities(const std::vector<Entity*>& newEntities)
{
    entities = newEntities;
    update();
}

void TempBoard::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    for (Entity* entity : entities) {
        if (!entity) continue;

        Position pos = entity->getPosition();


        int currentSize = tileSize;
        QColor currentQtColor = Qt::gray;


        if (Organism* org = dynamic_cast<Organism*>(entity)) {

            if (!org->getIsAlive()) continue;

            currentSize = org->getSize(); // Pobieramy rozmiar zmutowany w reproduce(

            Color c = org->getColor(); // Pobieramy kolor zmutowany w reproduce()
            currentQtColor = QColor(c.r, c.g, c.b);
        }


        painter.setBrush(QBrush(currentQtColor));
        painter.setPen(Qt::NoPen);

        // Rysujemy kółko w odpowiednim miejscu na siatce o dynamicznym rozmiarze
        painter.drawEllipse(
            pos.x * tileSize,
            pos.y * tileSize,
            currentSize,
            currentSize
            );

        // Opcjonalnie: mała obwódka dla drapieżników, żeby ich odróżnić
        // if (dynamic_cast<Predator*>(entity)) {
        //     painter.setPen(QPen(Qt::white, 1));
        //     painter.setBrush(Qt::NoBrush);
        //     painter.drawEllipse(pos.x * tileSize, pos.y * tileSize, currentSize, currentSize);
        // }
    }
}