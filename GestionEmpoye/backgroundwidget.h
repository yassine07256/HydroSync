#pragma once
#include <QPixmap>
#include <QWidget>

// Widget qui dessine background.png en mode « cover » (remplit la zone sans déformer l'image).
class BackgroundWidget : public QWidget
{
    Q_OBJECT
public:
    explicit BackgroundWidget(const QString &resourcePath, QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    QPixmap m_source;
    QPixmap m_scaled;
};
