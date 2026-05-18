#include "tempboard.h"
#include "simulationapp.h"
#include <QPainter>
#include <QPolygon>
#include <QRadialGradient>
#include <algorithm>
#include <cmath>

namespace {

QPolygon buildJaggedOutline(int centerX, int centerY, int radius, int teeth = 12)
{
    QPolygon outline;
    outline.reserve(teeth * 2);
    for (int i = 0; i < teeth * 2; ++i) {
        const double angle = 3.14159265358979323846 * i / teeth;
        const int r = (i % 2 == 0) ? radius + 2 : std::max(2, radius - 2);
        outline << QPoint(centerX + static_cast<int>(r * std::cos(angle)),
                          centerY + static_cast<int>(r * std::sin(angle)));
    }
    return outline;
}

} // namespace


TempBoard::TempBoard(QWidget* parent)
    : QWidget(parent)
    , tileSize(20)
    , animTimer_(new QTimer(this))
{
    animTimer_->setInterval(ANIM_INTERVAL_MS);
    connect(animTimer_, &QTimer::timeout, this, &TempBoard::advanceParticles);
}

void TempBoard::setSnapshot(const BoardSnapshot& newSnapshot)
{
    // Dodaj cząsteczki śmierci dla pozycji z tego ticku
    for (const Position& pos : newSnapshot.deathPositions) {
        DeathParticle dp;
        dp.center     = QPointF(pos.x * tileSize + tileSize * 0.2,
                                pos.y * tileSize + tileSize * 0.2);
        dp.progress   = 0.0;
        dp.baseRadius = tileSize / 3 + 3;
        deathParticles_.append(dp);
    }

    if (!deathParticles_.isEmpty() && !animTimer_->isActive())
        animTimer_->start();

    snapshot = newSnapshot;
    update();
}

void TempBoard::setShowEnvironmentParameters(bool show)
{
    if (showEnvironment == show) return;
    showEnvironment = show;
    update();
}

// ---------------------------------------------------------------------------
void TempBoard::advanceParticles()
{
    for (auto& dp : deathParticles_)
        dp.progress += ANIM_STEP;

    // Usuń zakończone
    deathParticles_.erase(
        std::remove_if(deathParticles_.begin(), deathParticles_.end(),
                       [](const DeathParticle& dp) { return dp.progress >= 1.0; }),
        deathParticles_.end());

    if (deathParticles_.isEmpty())
        animTimer_->stop();

    update();
}

// ---------------------------------------------------------------------------
void TempBoard::drawDeathParticle(QPainter& p, const DeathParticle& dp) const
{
    const qreal t  = dp.progress;          // 0→1
    const qreal r1 = dp.baseRadius * (1.0 + t * 2.8);  // rosnący promień zewnętrzny
    const qreal r2 = r1 * 0.55;                          // wewnętrzny promień (puste środek)

    // Fala 1 — główny pierścień (bańka)
    {
        const int alpha = static_cast<int>(220 * (1.0 - t));
        const qreal penW = std::max(1.0, (1.0 - t) * 3.5);
        p.setPen(QPen(QColor(255, 255, 255, alpha), penW));
        p.setBrush(Qt::NoBrush);
        p.drawEllipse(dp.center, r1, r1);
    }

    // Fala 2 — wewnętrzna, lekko opóźniona (efekt "pęknięcia")
    if (t > 0.12) {
        const qreal t2    = (t - 0.12) / 0.88;
        const qreal r2w   = dp.baseRadius * (1.0 + t2 * 1.6);
        const int   alpha = static_cast<int>(160 * (1.0 - t2));
        p.setPen(QPen(QColor(200, 230, 255, alpha), 1.5));
        p.setBrush(Qt::NoBrush);
        p.drawEllipse(dp.center, r2w, r2w);
    }

    // Rozbłysk w środku — mały jaśniejszy okrąg na początku
    if (t < 0.35) {
        const qreal tf    = t / 0.35;
        const qreal flash = dp.baseRadius * (1.0 - tf) * 0.7;
        const int   alpha = static_cast<int>(200 * (1.0 - tf));
        p.setPen(Qt::NoPen);
        p.setBrush(QColor(255, 245, 200, alpha));
        p.drawEllipse(dp.center, flash, flash);
    }

    // Drobiny — 8 "odłamków" lecących na zewnątrz
    const int nSparks = 8;
    for (int i = 0; i < nSparks; ++i) {
        const double angle = 2.0 * M_PI * i / nSparks;
        const qreal dist   = r1 * 0.85;
        const int   alpha  = static_cast<int>(180 * (1.0 - t));
        const qreal sr     = std::max(1.0, (1.0 - t) * 3.0);
        QPointF sparkPos(dp.center.x() + dist * std::cos(angle),
                         dp.center.y() + dist * std::sin(angle));
        p.setPen(Qt::NoPen);
        p.setBrush(QColor(255, 220, 180, alpha));
        p.drawEllipse(sparkPos, sr, sr);
    }
}

// ---------------------------------------------------------------------------
QSize TempBoard::sizeHint() const
{
    if (snapshot.isEmpty()) return QSize(400, 300);
    return QSize(snapshot.width * tileSize, snapshot.height * tileSize);
}

// ---------------------------------------------------------------------------
void TempBoard::paintEvent(QPaintEvent*)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    p.fillRect(rect(), QColor(30, 30, 30));

    if (snapshot.isEmpty()) {
        p.setPen(Qt::white);
        p.drawText(rect(), Qt::AlignCenter, "Oczekiwanie na dane...");
        // Ramka nawet przy pustym widoku
        p.setPen(QPen(QColor(80, 120, 160), 2));
        p.setBrush(Qt::NoBrush);
        p.drawRect(rect().adjusted(1, 1, -1, -1));
        return;
    }

    // Wyśrodkowanie planszy w widgecie
    const int boardW   = snapshot.width  * tileSize;
    const int boardH   = snapshot.height * tileSize;
    const int offsetX  = std::max(0, (width()  - boardW) / 2);
    const int offsetY  = std::max(0, (height() - boardH) / 2);

    p.translate(offsetX, offsetY);

    // ------------------------------------------------------------------
    // Płytki
    // ------------------------------------------------------------------
    for (const BoardTileSnapshot& tile : snapshot.tiles) {
        int x = tile.position.x * tileSize;
        int y = tile.position.y * tileSize;

        switch (tile.kind) {
        case BoardTileKind::Normal:
            p.fillRect(x, y, tileSize, tileSize, QColor(70, 70, 70));
            p.setPen(QPen(QColor(50, 50, 50), 0.5));
            p.drawRect(x, y, tileSize, tileSize);
            break;
        case BoardTileKind::Abundant:
            p.fillRect(x, y, tileSize, tileSize, QColor(100, 180, 100));
            break;
        case BoardTileKind::Poison:
            p.fillRect(x, y, tileSize, tileSize, QColor(180, 80, 80));
            break;
        case BoardTileKind::Impassable:
            p.fillRect(x, y, tileSize, tileSize, QColor(30, 30, 30));
            break;
        }

        if (showEnvironment) {
            if      (tile.temperature <= 10.0) p.fillRect(x, y, tileSize, tileSize, QColor( 60, 120, 255, 70));
            else if (tile.temperature >= 30.0) p.fillRect(x, y, tileSize, tileSize, QColor(255, 170,  50, 65));

            if      (tile.humidity >= 65.0)    p.fillRect(x, y, tileSize, tileSize, QColor( 40, 170, 200, 45));
            else if (tile.humidity <= 35.0)    p.fillRect(x, y, tileSize, tileSize, QColor(150, 110,  70, 40));

            if      (tile.sunlight >= 65.0)    p.fillRect(x, y, tileSize, tileSize, QColor(255, 235, 120, 35));
            else if (tile.sunlight <= 35.0)    p.fillRect(x, y, tileSize, tileSize, QColor( 40,  40,  70, 50));
        }
    }

    // ------------------------------------------------------------------
    // Organizmy (z interpolacją ruchu)
    // ------------------------------------------------------------------
    double t = simApp ? simApp->getInterpolation() : 1.0;

    for (const BoardOrganismSnapshot& org : snapshot.organisms) {
        Position curr = org.position;
        Position last = org.lastPosition;

        double drawX = curr.x;
        double drawY = curr.y;
        if (last.x != curr.x || last.y != curr.y) {
            drawX = last.x + (curr.x - last.x) * t;
            drawY = last.y + (curr.y - last.y) * t;
        }

        Color c       = org.color;
        int baseSize  = std::clamp(org.size, 1, 10);
        int sz        = 6 + baseSize * 2;

        int px = static_cast<int>(drawX * tileSize);
        int py = static_cast<int>(drawY * tileSize);

        // Cień
        p.setBrush(QColor(0, 0, 0, 80));
        p.setPen(Qt::NoPen);
        p.drawEllipse(px + 2, py + 2, sz, sz);

        // Ciało
        p.setBrush(QColor(c.r, c.g, c.b));
        p.setPen(Qt::NoPen);
        p.drawEllipse(px, py, sz, sz);

        if (org.predator) {
            p.setPen(QPen(Qt::white, 2));
            p.setBrush(Qt::NoBrush);
            p.drawEllipse(px + 1, py + 1, sz - 2, sz - 2);
        }

        int cx = px + sz / 2;
        int cy = py + sz / 2;

        if (org.omnivore) {
            const int outlineRadius = std::max(4, sz / 2 + 1);
            p.setPen(QPen(Qt::white, 2));
            p.setBrush(Qt::NoBrush);
            p.drawPolygon(buildJaggedOutline(cx, cy, outlineRadius));
        }

        int marker = std::max(4, sz / 3);
        p.setPen(QPen(Qt::white, 2));
        p.setBrush(Qt::NoBrush);

        if (org.temperaturePreference == TemperaturePreference::Thermophile) {
            p.drawLine(cx - marker, cy, cx + marker, cy);
            p.drawLine(cx, cy - marker, cx, cy + marker);
        } else if (org.temperaturePreference == TemperaturePreference::Cryophile) {
            QPoint diamond[4] = {
                {cx,          cy - marker},
                {cx + marker, cy},
                {cx,          cy + marker},
                {cx - marker, cy}
            };
            p.drawPolygon(diamond, 4);
        }
    }

    // ------------------------------------------------------------------
    // Animacje śmierci
    // ------------------------------------------------------------------
    for (const DeathParticle& dp : deathParticles_)
        drawDeathParticle(p, dp);

    // ------------------------------------------------------------------
    // Ramka planszy
    // ------------------------------------------------------------------
    {
        const QRect bdr = QRect(0, 0, boardW, boardH).adjusted(0, 0, -1, -1);
        p.setBrush(Qt::NoBrush);

        // Zewnętrzna ramka — gruba, ciemna
        p.setPen(QPen(QColor(20, 20, 25), 3));
        p.drawRect(bdr);

        // Wewnętrzna ramka — jasna, cienka (efekt "bevel")
        p.setPen(QPen(QColor(100, 140, 180, 200), 1));
        p.drawRect(bdr.adjusted(2, 2, -2, -2));
    }

    // Zaplanuj kolejne odświeżenie animacji ruchu
    if (simApp && t < 1.0)
        QTimer::singleShot(16, this, QOverload<>::of(&QWidget::update));
}
