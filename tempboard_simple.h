// ALTERNATYWNA PROSTSZA WERSJA - bez QElapsedTimer
// Zamiast mierzyć czas, po prostu liczymy klatki

#ifndef TEMPBOARD_H
#define TEMPBOARD_H

#include <QWidget>
#include <vector>
#include "entity.h"

class Tile;
class Entity;
class TempBoard : public QWidget
{
    Q_OBJECT
public:
    explicit TempBoard(QWidget *parent = nullptr);

    void setEntities(const std::vector<Entity*>& newEntities);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    std::vector<Entity*> entities;
    int tileSize;
    
    // Prostsza animacja
    int animFrame;       // Aktualna klatka animacji (0-12)
    int maxFrames;       // Ile klatek ma trwać animacja

};

#endif // TEMPBOARD_H
