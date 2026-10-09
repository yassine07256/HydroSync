#include "glasspanel.h"

#include <QPainter>

GlassPanel::GlassPanel(QWidget *parent, int alpha, int radius)
    : QFrame(parent), m_alpha(alpha), m_radius(radius)
{
    setAttribute(Qt::WA_StyledBackground, false);
}

void GlassPanel::setAlpha(int alpha)
{
    m_alpha = alpha;
    update();
}

void GlassPanel::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    p.setPen(QPen(QColor(255, 255, 255, qMin(255, m_alpha + 30)), 1));
    p.setBrush(QColor(255, 255, 255, m_alpha));
    p.drawRoundedRect(QRectF(rect()).adjusted(0.5, 0.5, -0.5, -0.5), m_radius, m_radius);
}
