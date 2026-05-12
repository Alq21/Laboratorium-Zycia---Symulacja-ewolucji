
#pragma once
#include <QWidget>
#include <vector>
#include "entity.h"
// to też tymczasowa klasa bym se testowała swoje organizmy do póki nie bedzie planszy
class TempBoard : public QWidget {
    Q_OBJECT

private:
    std::vector<Entity*> entities;
    const int tileSize = 20;

public:
    explicit TempBoard(QWidget *parent = nullptr);


    void setEntities(const std::vector<Entity*>& newEntities);

protected:

    void paintEvent(QPaintEvent *event) override;
};