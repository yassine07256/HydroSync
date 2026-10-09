#include "backgroundwidget.h"

#include <QPainter>

BackgroundWidget::BackgroundWidget(const QString &resourcePath, QWidget *parent)
    : QWidget(parent), m_source(resourcePath)
{
    setAutoFillBackground(false);
}

void BackgroundWidget::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    m_scaled = QPixmap(); // sera recalculée au prochain paintEvent
}

void BackgroundWidget::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    if (m_source.isNull()) {
        p.fillRect(rect(), QColor(0xd6, 0xe6, 0xf2));
        return;
    }
    const qreal dpr = devicePixelRatioF();
    const QSize target = size() * dpr;
    if (m_scaled.isNull() || m_scaled.size() != target) {
        m_scaled = m_source.scaled(target, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
        m_scaled.setDevicePixelRatio(dpr);
    }
    // Centre l'image et rogne ce qui dépasse.
    const QSizeF logical = QSizeF(m_scaled.size()) / dpr;
    p.drawPixmap(QPointF((width() - logical.width()) / 2.0, (height() - logical.height()) / 2.0), m_scaled);
}
