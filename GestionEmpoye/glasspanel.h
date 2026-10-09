#pragma once
#include <QFrame>

// Panneau blanc SEMI-TRANSPARENT dessiné avec QPainter (alpha 0 = invisible, 255 = opaque).
// Les widgets enfants restent opaques pour leur texte, mais le fond laisse voir background.png.
class GlassPanel : public QFrame
{
    Q_OBJECT
public:
    explicit GlassPanel(QWidget *parent = nullptr, int alpha = 215, int radius = 10);
    void setAlpha(int alpha);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    int m_alpha;
    int m_radius;
};
