#pragma once
#include <QColor>
#include <QList>
#include <QStringList>
#include <QWidget>

// Graphique en anneau (donut) avec libellés autour.
class DonutChart : public QWidget
{
    Q_OBJECT
public:
    struct Slice { QString label; double value; QColor color; };
    explicit DonutChart(QWidget *parent = nullptr);
    void setSlices(const QList<Slice> &slices);
    QSize minimumSizeHint() const override { return QSize(220, 190); }

protected:
    void paintEvent(QPaintEvent *) override;

private:
    QList<Slice> m_slices;
};

// Graphique à barres avec ligne de seuil (surcharge).
class BarChart : public QWidget
{
    Q_OBJECT
public:
    explicit BarChart(QWidget *parent = nullptr);
    void setData(const QStringList &labels, const QList<double> &values);
    void setThreshold(double value, const QString &legend);
    QSize minimumSizeHint() const override { return QSize(260, 190); }

protected:
    void paintEvent(QPaintEvent *) override;

private:
    QStringList m_labels;
    QList<double> m_values;
    double m_threshold = 0;
    QString m_thresholdLegend;
};

// Indicateur circulaire (anneau de progression) avec pourcentage au centre et libellé dessous.
class RingGauge : public QWidget
{
    Q_OBJECT
public:
    RingGauge(int percent, const QColor &color, const QString &label, QWidget *parent = nullptr);
    QSize minimumSizeHint() const override { return QSize(80, 96); }

protected:
    void paintEvent(QPaintEvent *) override;

private:
    int m_percent;
    QColor m_color;
    QString m_label;
};
