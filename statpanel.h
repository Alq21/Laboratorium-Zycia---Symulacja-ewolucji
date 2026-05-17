#ifndef STATPANEL_H
#define STATPANEL_H

#include <QWidget>
#include "simulationapp.h"

QT_BEGIN_NAMESPACE
class QChartView;
class QChart;
class QLineSeries;
class QValueAxis;
class QLabel;
QT_END_NAMESPACE

/**
 * StatPanel — zakładka "Statystyki".
 *
 * Trzy wykresy (Qt Charts):
 *  1) Populacja: drapieżniki | producenci | wszystkożercy
 *  2) Preferencje temperatury: Cryophile | Thermophile | Default
 *  3) Średni rozmiar i szybkość
 *
 * Wykresy 2 i 3 używają "napiętej" osi Y (tight range) — widać rzeczywistą zmienność,
 * nawet gdy wartości różnią się tylko nieznacznie.
 */
class StatPanel : public QWidget
{
    Q_OBJECT

public:
    explicit StatPanel(QWidget* parent = nullptr);

    void refresh(const StatsData& data);

private:
    // Wykres 1 — Populacja
    QChart*      chartPop;
    QLineSeries* seriesPredator;
    QLineSeries* seriesProducer;
    QLineSeries* seriesOmnivore;
    QValueAxis*  axisXPop;
    QValueAxis*  axisYPop;
    QChartView*  viewPop;

    // Wykres 2 — Preferencje temp
    QChart*      chartTemp;
    QLineSeries* seriesCryo;
    QLineSeries* seriesThermo;
    QLineSeries* seriesDefault;
    QValueAxis*  axisXTemp;
    QValueAxis*  axisYTemp;
    QChartView*  viewTemp;

    // Wykres 3 — Srednie cechy
    QChart*      chartTraits;
    QLineSeries* seriesAvgSize;
    QLineSeries* seriesAvgSpeed;
    QValueAxis*  axisXTraits;
    QValueAxis*  axisYTraits;
    QChartView*  viewTraits;

    QLabel*      labelNoData;

    void buildCharts();
};

#endif // STATPANEL_H
