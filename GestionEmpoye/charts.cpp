#include "charts.h"

#include <QPainter>
#include <QPainterPath>
#include <QtMath>

// ------------------------------------------------------------------ DonutChart
DonutChart::DonutChart(QWidget *parent) : QWidget(parent) {}

void DonutChart::setSlices(const QList<Slice> &slices)
{
    m_slices = slices;
    update();
}

void DonutChart::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    double total = 0;
    for (const Slice &s : m_slices) total += s.value;
    if (total <= 0) return;

    const double side = qMin(width() * 0.46, height() * 0.74);
    const QPointF c(width() / 2.0, height() / 2.0);
    const QRectF outer(c.x() - side / 2, c.y() - side / 2, side, side);
    const QRectF inner = outer.adjusted(side * 0.27, side * 0.27, -side * 0.27, -side * 0.27);

    QPainterPath ring;
    ring.addEllipse(outer);
    QPainterPath hole;
    hole.addEllipse(inner);
    p.save();
    p.setClipPath(ring.subtracted(hole));
    double start = 90 * 16;
    QList<double> mids;
    for (const Slice &s : m_slices) {
        const double span = -s.value / total * 360 * 16;
        p.setPen(QPen(Qt::white, 1.5));
        p.setBrush(s.color);
        p.drawPie(outer, int(start), int(span));
        mids << start + span / 2;
        start += span;
    }
    p.restore();

    QFont f = font();
    f.setPointSizeF(8.5);
    p.setFont(f);
    p.setPen(QColor(0x22, 0x2b, 0x36));
    for (int i = 0; i < m_slices.size(); ++i) {
        const double rad = qDegreesToRadians(mids[i] / 16.0);
        const double cs = qCos(rad), sn = -qSin(rad);
        const QPointF pt = c + QPointF(cs, sn) * (side / 2 + 8);
        const QRectF box(0, 0, 92, 34);
        QRectF r = box;
        int flags = Qt::TextWordWrap;
        if (cs > 0.35)       { r.moveTopLeft(QPointF(pt.x(), pt.y() - 17)); flags |= Qt::AlignLeft | Qt::AlignVCenter; }
        else if (cs < -0.35) { r.moveTopRight(QPointF(pt.x(), pt.y() - 17)); flags |= Qt::AlignRight | Qt::AlignVCenter; }
        else if (sn < 0)     { r.moveBottomLeft(QPointF(pt.x() - 46, pt.y())); flags |= Qt::AlignHCenter | Qt::AlignBottom; }
        else                 { r.moveTopLeft(QPointF(pt.x() - 46, pt.y())); flags |= Qt::AlignHCenter | Qt::AlignTop; }
        p.drawText(r, flags, m_slices[i].label);
    }
}

// -------------------------------------------------------------------- BarChart
BarChart::BarChart(QWidget *parent) : QWidget(parent) {}

void BarChart::setData(const QStringList &labels, const QList<double> &values)
{
    m_labels = labels;
    m_values = values;
    update();
}

void BarChart::setThreshold(double value, const QString &legend)
{
    m_threshold = value;
    m_thresholdLegend = legend;
    update();
}

void BarChart::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    const double maxV = 30;
    const QRectF plot(30, 22, width() - 40, height() - 22 - 58);
    QFont f = font();
    f.setPointSizeF(7.5);
    p.setFont(f);
    p.setPen(QColor(0x33, 0x3b, 0x45));

    // axe vertical + graduations
    p.drawLine(plot.bottomLeft(), plot.topLeft());
    for (int v = 0; v <= maxV; v += 5) {
        const double y = plot.bottom() - v / maxV * plot.height();
        p.drawLine(QPointF(plot.left() - 3, y), QPointF(plot.left(), y));
        p.drawText(QRectF(0, y - 7, plot.left() - 6, 14), Qt::AlignRight | Qt::AlignVCenter, QString::number(v));
    }
    p.drawLine(plot.bottomLeft(), plot.bottomRight());

    const int n = m_values.size();
    if (n > 0) {
        const double slot = plot.width() / n;
        const double bw = slot * 0.62;
        for (int i = 0; i < n; ++i) {
            const double h = m_values[i] / maxV * plot.height();
            const QRectF bar(plot.left() + i * slot + (slot - bw) / 2, plot.bottom() - h, bw, h);
            p.fillRect(bar, QColor(0x1f, 0x6f, 0xae));

            p.save();
            p.translate(bar.center().x() + 3, plot.bottom() + 5);
            p.rotate(-55);
            p.setPen(QColor(0x33, 0x3b, 0x45));
            p.drawText(QRectF(-60, -7, 60, 14), Qt::AlignRight | Qt::AlignVCenter,
                       i < m_labels.size() ? m_labels[i] : QString());
            p.restore();
        }
    }

    if (m_threshold > 0) {
        const double y = plot.bottom() - m_threshold / maxV * plot.height();
        p.setPen(QPen(QColor(0xe0, 0x4b, 0x4b), 1.6));
        p.drawLine(QPointF(plot.left(), y), QPointF(plot.right(), y));
        p.setPen(QColor(0x33, 0x3b, 0x45));
        const double lx = plot.right() - 135;
        p.setPen(QPen(QColor(0xe0, 0x4b, 0x4b), 2.2));
        p.drawLine(QPointF(lx, 9), QPointF(lx + 18, 9));
        p.setPen(QColor(0x33, 0x3b, 0x45));
        p.drawText(QRectF(lx + 22, 1, 115, 16), Qt::AlignLeft | Qt::AlignVCenter, m_thresholdLegend);
    }

    // légende du bas
    const double bx = width() / 2.0 - 45;
    p.fillRect(QRectF(bx, height() - 15, 14, 8), QColor(0x2a, 0x8f, 0xa6));
    p.drawText(QRectF(bx + 18, height() - 20, 90, 16), Qt::AlignLeft | Qt::AlignVCenter, QStringLiteral("Tâches actives"));
}

// ------------------------------------------------------------------- RingGauge
RingGauge::RingGauge(int percent, const QColor &color, const QString &label, QWidget *parent)
    : QWidget(parent), m_percent(percent), m_color(color), m_label(label)
{
}

void RingGauge::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    const double d = qMin(double(width()) - 8, height() - 24.0);
    const QRectF r((width() - d) / 2.0, 3, d, d);
    const double pw = qMax(5.0, d * 0.09);
    const QRectF arc = r.adjusted(pw / 2, pw / 2, -pw / 2, -pw / 2);

    p.setBrush(Qt::NoBrush);
    QColor track = m_color;
    track.setAlpha(60);
    p.setPen(QPen(track, pw, Qt::SolidLine, Qt::FlatCap));
    p.drawEllipse(arc);
    p.setPen(QPen(m_color, pw, Qt::SolidLine, Qt::RoundCap));
    p.drawArc(arc, 90 * 16, int(-m_percent / 100.0 * 360 * 16));

    QFont f = font();
    f.setPointSizeF(qMax(9.0, d * 0.19));
    p.setFont(f);
    p.setPen(QColor(0x22, 0x2b, 0x36));
    p.drawText(r, Qt::AlignCenter, QStringLiteral("%1%").arg(m_percent));

    f.setPointSizeF(8);
    p.setFont(f);
    p.drawText(QRectF(0, r.bottom() + 3, width(), 16), Qt::AlignHCenter | Qt::AlignTop, m_label);
}
