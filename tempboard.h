#ifndef TEMPBOARD_H
#define TEMPBOARD_H

#include <QWidget>
#include <vector>

class Entity;
class SimulationApp;

class TempBoard : public QWidget
{
    Q_OBJECT

public:
    explicit TempBoard(QWidget *parent = nullptr);
    void setEntities(const std::vector<Entity*>& newEntities);
    void setSimApp(SimulationApp* app) { simApp = app; }

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    std::vector<Entity*> entities;
    int tileSize = 20;
    SimulationApp* simApp = nullptr;
};

#endif // TEMPBOARD_H