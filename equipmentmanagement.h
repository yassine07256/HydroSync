#ifndef EQUIPMENTMANAGEMENT_H
#define EQUIPMENTMANAGEMENT_H

#include <QWidget>
#include <QTableWidget>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>

class EquipmentManagement : public QWidget
{
    Q_OBJECT

public:
    explicit EquipmentManagement(QWidget *parent = nullptr);

private slots:
    void ajouterEquipement();
    void modifierEquipement();
    void supprimerEquipement();
    void rechercherEquipement();
    void filtrerEquipement();
    void exporterEquipements();

private:
    void setupUI();
    void chargerDonneesTest();
    void mettreAJourStatistiques();
    void appliquerStyle();

    QLineEdit *searchEdit;

    QComboBox *etatCombo;
    QComboBox *typeCombo;

    QPushButton *btnAjouter;
    QPushButton *btnModifier;
    QPushButton *btnSupprimer;
    QPushButton *btnExporter;

    QTableWidget *tableEquipements;

    QLabel *totalLabel;
    QLabel *panneLabel;
    QLabel *serviceLabel;
    QLabel *alerteLabel;
};

#endif