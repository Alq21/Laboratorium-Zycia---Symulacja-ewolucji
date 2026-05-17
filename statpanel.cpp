#include "statpanel.h"

#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <algorithm>
#include <cmath>
#include <limits>

StatPanel::StatPanel(QWidget* parent)
    : QWidget(parent)
{
    buildCharts();
}

// ---------------------------------------------------------------------------
static QValueAxis* makeIntAxis(const QString& title)
{
    auto* ax = new QValueAxis;
    ax->setTitleText(title);
    ax->setLabelFormat("%d");
    ax->setTickCount(6);
    return ax;
}

static QValueAxis* makeDblAxis(const QString& title)
{
    auto* ax = new QValueAxis;
    ax->setTitleText(title);
    ax->setLabelFormat("%.1f");
    ax->setTickCount(6);
    return ax;
}

// Oblicz "napiętą" oś Y — zakres faktycznych danych + margines, min >= 0
static std::pair<double,double> tightRange(
    std::initializer_list<const std::vector<int>*>    intSeries,
    std::initializer_list<const std::vector<double>*> dblSeries = {},
    bool forceZeroMin = false)
{
    double vMin =  std::numeric_limits<double>::max();
    double vMax = -std::numeric_limits<double>::max();

    for (auto* s : intSeries)
        for (auto v : *s) { vMin = std::min(vMin, (double)v); vMax = std::max(vMax, (double)v); }
    for (auto* s : dblSeries)
        for (auto v : *s) { vMin = std::min(vMin, v);         vMax = std::max(vMax, v); }

    if (vMax <= vMin) { vMin = 0; vMax = 10; }

    const double span   = vMax - vMin;
    const double margin = std::max(span * 0.12, 0.5);

    double lo = forceZeroMin ? 0.0 : std::max(0.0, vMin - margin);
    double hi = vMax + margin;
    if (hi <= lo) hi = lo + 1;
    return {lo, hi};
}

// ---------------------------------------------------------------------------
void StatPanel::buildCharts()
{
    // -----------------------------------------------------------------------
    // Wykres 1 — Populacja
    // -----------------------------------------------------------------------
    seriesPredator = new QLineSeries; seriesPredator->setName(QStringLiteral("Drapiezniki"));
    seriesProducer = new QLineSeries; seriesProducer->setName(QStringLiteral("Producenci"));
    seriesOmnivore = new QLineSeries; seriesOmnivore->setName(QStringLiteral("Wszystkozerce"));

    QPen penPred(QColor(220, 60, 60));   penPred.setWidth(2);   seriesPredator->setPen(penPred);
    QPen penProd(QColor(60, 200, 60));   penProd.setWidth(2);   seriesProducer->setPen(penProd);
    QPen penOmni(QColor(70, 100, 230));  penOmni.setWidth(2);   seriesOmnivore->setPen(penOmni);

    chartPop = new QChart;
    chartPop->setTitle(QStringLiteral("Populacja organizmow"));
    chartPop->addSeries(seriesPredator);
    chartPop->addSeries(seriesProducer);
    chartPop->addSeries(seriesOmnivore);
    chartPop->legend()->setVisible(true);
    chartPop->legend()->setAlignment(Qt::AlignBottom);
    chartPop->setMargins(QMargins(4, 4, 4, 4));

    axisXPop = makeIntAxis(QStringLiteral("Tura"));
    axisYPop = makeIntAxis(QStringLiteral("Liczba organizmow"));
    chartPop->addAxis(axisXPop, Qt::AlignBottom);
    chartPop->addAxis(axisYPop, Qt::AlignLeft);
    seriesPredator->attachAxis(axisXPop); seriesPredator->attachAxis(axisYPop);
    seriesProducer->attachAxis(axisXPop); seriesProducer->attachAxis(axisYPop);
    seriesOmnivore->attachAxis(axisXPop); seriesOmnivore->attachAxis(axisYPop);

    viewPop = new QChartView(chartPop, this);
    viewPop->setRenderHint(QPainter::Antialiasing);

    // -----------------------------------------------------------------------
    // Wykres 2 — Preferencje temperatury
    // -----------------------------------------------------------------------
    seriesCryo    = new QLineSeries; seriesCryo->setName(QStringLiteral("Cryophile"));
    seriesThermo  = new QLineSeries; seriesThermo->setName(QStringLiteral("Thermophile"));
    seriesDefault = new QLineSeries; seriesDefault->setName(QStringLiteral("Default"));

    QPen penCryo(QColor(0, 180, 230));   penCryo.setWidth(2);   seriesCryo->setPen(penCryo);
    QPen penTherm(QColor(230, 100, 0));  penTherm.setWidth(2);  seriesThermo->setPen(penTherm);
    QPen penDef(QColor(170, 170, 170));  penDef.setWidth(2);    seriesDefault->setPen(penDef);

    chartTemp = new QChart;
    chartTemp->setTitle(QStringLiteral("Preferencje temperaturowe"));
    chartTemp->addSeries(seriesCryo);
    chartTemp->addSeries(seriesThermo);
    chartTemp->addSeries(seriesDefault);
    chartTemp->legend()->setVisible(true);
    chartTemp->legend()->setAlignment(Qt::AlignBottom);
    chartTemp->setMargins(QMargins(4, 4, 4, 4));

    axisXTemp = makeIntAxis(QStringLiteral("Tura"));
    axisYTemp = makeIntAxis(QStringLiteral("Liczba organizmow"));
    chartTemp->addAxis(axisXTemp, Qt::AlignBottom);
    chartTemp->addAxis(axisYTemp, Qt::AlignLeft);
    seriesCryo->attachAxis(axisXTemp);    seriesCryo->attachAxis(axisYTemp);
    seriesThermo->attachAxis(axisXTemp);  seriesThermo->attachAxis(axisYTemp);
    seriesDefault->attachAxis(axisXTemp); seriesDefault->attachAxis(axisYTemp);

    viewTemp = new QChartView(chartTemp, this);
    viewTemp->setRenderHint(QPainter::Antialiasing);

    // -----------------------------------------------------------------------
    // Wykres 3 — Srednie cechy
    // -----------------------------------------------------------------------
    seriesAvgSize  = new QLineSeries; seriesAvgSize->setName(QStringLiteral("Sredni rozmiar"));
    seriesAvgSpeed = new QLineSeries; seriesAvgSpeed->setName(QStringLiteral("Srednia szybkosc"));

    QPen penSz(QColor(190, 70, 200));   penSz.setWidth(2);   seriesAvgSize->setPen(penSz);
    QPen penSp(QColor(200, 170, 0));    penSp.setWidth(2);   seriesAvgSpeed->setPen(penSp);

    chartTraits = new QChart;
    chartTraits->setTitle(QStringLiteral("Sredni rozmiar i szybkosc"));
    chartTraits->addSeries(seriesAvgSize);
    chartTraits->addSeries(seriesAvgSpeed);
    chartTraits->legend()->setVisible(true);
    chartTraits->legend()->setAlignment(Qt::AlignBottom);
    chartTraits->setMargins(QMargins(4, 4, 4, 4));

    axisXTraits = makeIntAxis(QStringLiteral("Tura"));
    axisYTraits = makeDblAxis(QStringLiteral("Wartosc"));
    chartTraits->addAxis(axisXTraits, Qt::AlignBottom);
    chartTraits->addAxis(axisYTraits, Qt::AlignLeft);
    seriesAvgSize->attachAxis(axisXTraits);  seriesAvgSize->attachAxis(axisYTraits);
    seriesAvgSpeed->attachAxis(axisXTraits); seriesAvgSpeed->attachAxis(axisYTraits);

    viewTraits = new QChartView(chartTraits, this);
    viewTraits->setRenderHint(QPainter::Antialiasing);

    // -----------------------------------------------------------------------
    // Etykieta zastępcza (gdy brak danych)
    // -----------------------------------------------------------------------
    labelNoData = new QLabel(
        QStringLiteral("Brak danych — uruchom symulacje i zapauzuj, aby zobaczyc statystyki."),
        this);
    labelNoData->setAlignment(Qt::AlignCenter);
    labelNoData->setStyleSheet(QStringLiteral("color: gray; font-size: 14px;"));

    // -----------------------------------------------------------------------
    // Układ:  [Populacja | Temp-preferencje]  górny rząd (równej szerokości)
    //         [Srednie cechy — pełna szerokość]  dolny rząd
    // -----------------------------------------------------------------------
    auto* topRow = new QHBoxLayout;
    topRow->setSpacing(6);
    topRow->addWidget(viewPop,   1);
    topRow->addWidget(viewTemp,  1);

    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(8, 8, 8, 8);
    layout->setSpacing(6);
    layout->addWidget(labelNoData);
    layout->addLayout(topRow,    3);   // górny rząd — 3/5 wysokości
    layout->addWidget(viewTraits, 2);  // dolny —  2/5 wysokości

    viewPop->hide();
    viewTemp->hide();
    viewTraits->hide();
}

// ---------------------------------------------------------------------------
void StatPanel::refresh(const StatsData& data)
{
    if (!data.hasData || data.ticks.empty()) {
        labelNoData->show();
        viewPop->hide();
        viewTemp->hide();
        viewTraits->hide();
        return;
    }

    labelNoData->hide();
    viewPop->show();
    viewTemp->show();
    viewTraits->show();

    // Helper: wypełnia serie punktami
    auto fill = [&](QLineSeries* series, const auto& values) {
        series->clear();
        for (std::size_t i = 0; i < data.ticks.size() && i < values.size(); ++i)
            series->append(static_cast<qreal>(data.ticks[i]),
                           static_cast<qreal>(values[i]));
    };

    const double xMin = static_cast<double>(data.ticks.front());
    const double xMax = static_cast<double>(data.ticks.back());
    const double xHi  = xMax > xMin ? xMax : xMin + 1;

    // -----------------------------------------------------------------------
    // Wykres 1 — Populacja (Y zaczyna od 0, bo liczymy osobniki)
    // -----------------------------------------------------------------------
    fill(seriesPredator, data.predatorCounts);
    fill(seriesProducer, data.producerCounts);
    fill(seriesOmnivore, data.omnivoreCounts);

    axisXPop->setRange(xMin, xHi);
    {
        auto [lo, hi] = tightRange(
            {&data.predatorCounts, &data.producerCounts, &data.omnivoreCounts},
            {}, /*forceZeroMin=*/true);
        axisYPop->setRange(lo, hi);
    }

    // -----------------------------------------------------------------------
    // Wykres 2 — Preferencje temp (napięty zakres Y — pokazuje realne zmiany)
    // -----------------------------------------------------------------------
    fill(seriesCryo,    data.cryophileCounts);
    fill(seriesThermo,  data.thermophileCounts);
    fill(seriesDefault, data.defaultCounts);

    axisXTemp->setRange(xMin, xHi);
    {
        // Nie wymuszamy zera — jeśli wszystkie wartości są np. między 30 a 50,
        // oś Y skaluje się do [26, 54] a nie [0, 55].
        auto [lo, hi] = tightRange(
            {&data.cryophileCounts, &data.thermophileCounts, &data.defaultCounts},
            {}, /*forceZeroMin=*/false);
        axisYTemp->setRange(lo, hi);
    }

    // -----------------------------------------------------------------------
    // Wykres 3 — Srednie cechy (napięty zakres Y dla obu serii)
    // -----------------------------------------------------------------------
    fill(seriesAvgSize,  data.avgSizes);
    fill(seriesAvgSpeed, data.avgSpeeds);

    axisXTraits->setRange(xMin, xHi);
    {
        auto [lo, hi] = tightRange(
            {}, {&data.avgSizes, &data.avgSpeeds}, /*forceZeroMin=*/false);
        axisYTraits->setRange(lo, hi);
    }
}
