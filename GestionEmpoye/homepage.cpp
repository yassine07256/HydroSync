#include "homepage.h"
#include "charts.h"
#include "glasspanel.h"

#include <QFrame>
#include <QGridLayout>
#include <QLabel>
#include <QVBoxLayout>

namespace {
QLabel *makeLabel(const QString &text, double pt, bool bold = false)
{
    auto *l = new QLabel(text);
    QFont f = l->font();
    f.setPointSizeF(pt);
    f.setBold(bold);
    l->setFont(f);
    return l;
}

// Petite carte interne (sous-carte de « Vue d'Ensemble RH »).
QWidget *miniCard(const QString &caption, const QString &value)
{
    auto *card = new GlassPanel(nullptr, 235, 12);
    auto *lay = new QVBoxLayout(card);
    lay->setContentsMargins(14, 12, 14, 12);
    lay->addWidget(makeLabel(caption, 9.5), 0, Qt::AlignHCenter);
    { auto *v = makeLabel(value, 20); v->setAlignment(Qt::AlignCenter); lay->addWidget(v, 1); }
    return card;
}
} // namespace

QStringList HomePage::notifications()
{
    return {
        QStringLiteral("Modification apportée à la fiche de Léo Dubois (Service DevOps)."),
        QStringLiteral("Alerte de surcharge détectée pour Emma Petit (Service Développement)."),
        QStringLiteral("Nouvelle demande de mot de passe oublié pour Chloé Martin."),
        QStringLiteral("Export PDF de la liste des employés généré avec succès."),
    };
}

HomePage::HomePage(QWidget *parent) : QWidget(parent)
{
    auto *grid = new QGridLayout(this);
    grid->setContentsMargins(34, 20, 34, 20);
    grid->setSpacing(18);

    grid->addWidget(statsCard(),        0, 0, 1, 2);
    grid->addWidget(workloadCard(),     0, 2, 1, 2);
    grid->addWidget(performanceCard(),  0, 4, 1, 2);
    grid->addWidget(hrOverviewCard(),   1, 0, 1, 4);
    grid->addWidget(notificationsCard(),1, 4, 1, 2);
    grid->setRowStretch(0, 5);
    grid->setRowStretch(1, 4);
    for (int c = 0; c < 6; ++c) grid->setColumnStretch(c, 1);
}

QWidget *HomePage::statsCard()
{
    auto *card = new GlassPanel(nullptr, 215);
    auto *lay = new QVBoxLayout(card);
    lay->setContentsMargins(14, 10, 14, 10);
    lay->addWidget(makeLabel(QStringLiteral("Statistiques des Employés"), 12.5));
    lay->addWidget(makeLabel(QStringLiteral("Progression Hebdomadaire"), 11));

    auto *chart = new DonutChart;
    chart->setSlices({
        { QStringLiteral("Développeurs"),              32, QColor(0x2a, 0x9d, 0x9a) },
        { QStringLiteral("Ingénieurs\nDevOps"),        24, QColor(0x1f, 0x6f, 0xae) },
        { QStringLiteral("Designers\nUX"),             18, QColor(0x2d, 0x87, 0xc8) },
        { QStringLiteral("Marketing"),                 20, QColor(0x25, 0x3c, 0x78) },
        { QStringLiteral("RH"),                         6, QColor(0xf0, 0xb4, 0x29) },
    });
    lay->addWidget(chart, 1);
    return card;
}

QWidget *HomePage::workloadCard()
{
    auto *card = new GlassPanel(nullptr, 215);
    auto *lay = new QVBoxLayout(card);
    lay->setContentsMargins(14, 10, 14, 10);
    lay->addWidget(makeLabel(QStringLiteral("Charge de Travail des Employés"), 12.5));
    lay->addWidget(makeLabel(QStringLiteral("Prochains Cours"), 11));

    auto *chart = new BarChart;
    chart->setData(
        { "Thomas", "Emma", "Chloé", "Dubois", "Léo", "Ben", "Thomas", "Emma", "Chloé", "Léo", "Ben", "Anni" },
        { 29, 24, 13, 20, 27, 14, 10, 20, 24, 10, 5, 8 });
    chart->setThreshold(25, QStringLiteral("Surcharge de Travail"));
    lay->addWidget(chart, 1);
    return card;
}

QWidget *HomePage::performanceCard()
{
    auto *card = new GlassPanel(nullptr, 215);
    auto *lay = new QVBoxLayout(card);
    lay->setContentsMargins(14, 10, 14, 10);
    lay->addWidget(makeLabel(QStringLiteral("Performances Globales"), 12.5));

    auto *grid = new QGridLayout;
    grid->setSpacing(6);
    grid->addWidget(new RingGauge(75, QColor(0xe8, 0x52, 0x7c), QStringLiteral("Participation")), 0, 0);
    grid->addWidget(new RingGauge(95, QColor(0x3c, 0xb9, 0x8b), QStringLiteral("Devoirs Rendus")), 0, 1);
    grid->addWidget(new RingGauge(82, QColor(0xf2, 0xc2, 0x30), QStringLiteral("Note Globale")), 0, 2);
    grid->addWidget(new RingGauge(82, QColor(0xf2, 0xc2, 0x30), QStringLiteral("Évaluation")), 1, 1);
    lay->addLayout(grid, 1);
    return card;
}

QWidget *HomePage::hrOverviewCard()
{
    auto *card = new GlassPanel(nullptr, 215);
    auto *lay = new QVBoxLayout(card);
    lay->setContentsMargins(18, 12, 18, 14);
    lay->addWidget(makeLabel(QStringLiteral("Vue d'Ensemble RH"), 12.5));

    auto *grid = new QGridLayout;
    grid->setSpacing(14);
    grid->addWidget(miniCard(QStringLiteral("Nombre d'Employés Actifs :"), QStringLiteral("145")), 0, 0);
    grid->addWidget(miniCard(QStringLiteral("Performance Moyenne par Service :"), QStringLiteral("88%")), 1, 0);
    grid->addWidget(miniCard(QStringLiteral("Demandes en Attente :"), QStringLiteral("7\n(Recrutement)")), 0, 1, 2, 1);
    grid->setColumnStretch(0, 1);
    grid->setColumnStretch(1, 1);
    lay->addLayout(grid, 1);
    return card;
}

QWidget *HomePage::notificationsCard()
{
    auto *card = new GlassPanel(nullptr, 225);
    auto *lay = new QVBoxLayout(card);
    lay->setContentsMargins(14, 10, 14, 10);
    lay->addWidget(makeLabel(QStringLiteral("Notifications Récentes"), 12));

    const QStringList items = notifications();
    for (int i = 0; i < items.size(); ++i) {
        auto *l = makeLabel(items.at(i), 9);
        l->setWordWrap(true);
        lay->addWidget(l);
        if (i < items.size() - 1) {
            auto *line = new QFrame;
            line->setFrameShape(QFrame::HLine);
    line->setObjectName(QStringLiteral("separator"));
            lay->addWidget(line);
        }
    }
    lay->addStretch(1);
    return card;
}
