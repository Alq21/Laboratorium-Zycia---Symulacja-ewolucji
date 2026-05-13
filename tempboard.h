#ifndef TEMPBOARD_H
#define TEMPBOARD_H

#include <QWidget>
#include <vector>
#include "entity.h"

class TempBoard : public QWidget
{
    Q_OBJECT
public:
    explicit TempBoard(QWidget *parent = nullptr);

    // Ta funkcja przyjmuje listę organizmów do narysowania
    void setEntities(const std::vector<Entity*>& newEntities);

protected:
    // Ta funkcja odpowiada za fizyczne rysowanie na ekranie
    void paintEvent(QPaintEvent *event) override;

private:
    std::vector<Entity*> entities;
    int tileSize; // Podstawa siatki, np. 20 pikseli
};

#endif // TEMPBOARD_H