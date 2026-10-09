#pragma once
#include <QStringList>
#include <QWidget>

// Page « Accueil » (image 1) : tableau de bord avec cartes semi-transparentes.
class HomePage : public QWidget
{
    Q_OBJECT
public:
    explicit HomePage(QWidget *parent = nullptr);

    // Texte des notifications récentes (réutilisé par l'icône cloche de la barre supérieure).
    static QStringList notifications();

private:
    QWidget *statsCard();
    QWidget *workloadCard();
    QWidget *performanceCard();
    QWidget *hrOverviewCard();
    QWidget *notificationsCard();
};
