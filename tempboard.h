#ifndef TEMPBOARD_H
#define TEMPBOARD_H

#include <QWidget>
#include <QTimer>
#include <QList>
#include <QPointF>
#include "boardsnapshot.h"

class SimulationApp;

// Cząsteczka animacji pękającej bańki
struct DeathParticle {
    QPointF center;     // środek w pikselach
    qreal   progress;   // 0.0 → 1.0 (postęp animacji)
    int     baseRadius; // promień startowy (odpowiada rozmiarowi organizmu)
};

class TempBoard : public QWidget
{
    Q_OBJECT

public:
    explicit TempBoard(QWidget* parent = nullptr);

    void setSnapshot(const BoardSnapshot& newSnapshot);
    void setSimApp(SimulationApp* app) { simApp = app; }
    void setShowEnvironmentParameters(bool show);
    bool showEnvironmentParameters() const { return showEnvironment; }
    QSize sizeHint() const override;
    void setWorldSize(int width, int height);

protected:
    void paintEvent(QPaintEvent* event) override;

private slots:
    void advanceParticles();

private:
    void drawDeathParticle(QPainter& p, const DeathParticle& dp) const;

    BoardSnapshot snapshot;
    int tileSize = 24;
    SimulationApp* simApp = nullptr;
    bool showEnvironment  = false;
    int worldWidth = 80;
int worldHeight = 60;
    // Animacja śmierci
    QList<DeathParticle> deathParticles_;
    QTimer*              animTimer_;

    static constexpr int   ANIM_INTERVAL_MS = 16;   // ~60 fps
    static constexpr qreal ANIM_STEP        = 0.055; // kroki postępu (~18 klatek)
};

#endif // TEMPBOARD_H
