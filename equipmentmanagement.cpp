#include "equipmentmanagement.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QMessageBox>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QHeaderView>

EquipmentManagement::EquipmentManagement(QWidget *parent)
    : QWidget(parent)
{
    setupUI();
    chargerDonneesTest();
    mettreAJourStatistiques();
    appliquerStyle();
}

void EquipmentManagement::setupUI()
{
    // ==============================
    // TITRE
    // ==============================

    QLabel *title = new QLabel("Gestion des équipements");
    title->setObjectName("title");

    QLabel *subtitle = new QLabel(
        "Gérer, rechercher et surveiller les équipements de l'usine"
        );

    // ==============================
    // RECHERCHE
    // ==============================

    searchEdit = new QLineEdit();
    searchEdit->setPlaceholderText("Rechercher par ID ou type...");

    connect(searchEdit, &QLineEdit::textChanged,
            this, &EquipmentManagement::rechercherEquipement);

    // ==============================
    // FILTRES
    // ==============================

    etatCombo = new QComboBox();
    etatCombo->addItem("Tous");
    etatCombo->addItem("En service");
    etatCombo->addItem("Maintenance");
    etatCombo->addItem("En panne");

    typeCombo = new QComboBox();
    typeCombo->addItem("Tous");
    typeCombo->addItem("Pompe");
    typeCombo->addItem("Filtre");
    typeCombo->addItem("Lampe UV");
    typeCombo->addItem("Vanne");

    connect(etatCombo, &QComboBox::currentTextChanged,
            this, &EquipmentManagement::filtrerEquipement);

    connect(typeCombo, &QComboBox::currentTextChanged,
            this, &EquipmentManagement::filtrerEquipement);

    // ==============================
    // BOUTONS
    // ==============================

    btnAjouter = new QPushButton("+ Ajouter");
    btnModifier = new QPushButton("Modifier");
    btnSupprimer = new QPushButton("Supprimer");
    btnExporter = new QPushButton("Exporter");

    connect(btnAjouter, &QPushButton::clicked,
            this, &EquipmentManagement::ajouterEquipement);

    connect(btnModifier, &QPushButton::clicked,
            this, &EquipmentManagement::modifierEquipement);

    connect(btnSupprimer, &QPushButton::clicked,
            this, &EquipmentManagement::supprimerEquipement);

    connect(btnExporter, &QPushButton::clicked,
            this, &EquipmentManagement::exporterEquipements);

    // ==============================
    // STATISTIQUES
    // ==============================

    totalLabel = new QLabel("0");
    panneLabel = new QLabel("0");
    serviceLabel = new QLabel("0");
    alerteLabel = new QLabel("0");

    totalLabel->setObjectName("statValue");
    panneLabel->setObjectName("statValue");
    serviceLabel->setObjectName("statValue");
    alerteLabel->setObjectName("statValue");

    QGridLayout *statsLayout = new QGridLayout();

    statsLayout->addWidget(new QLabel("Total"), 0, 0);
    statsLayout->addWidget(totalLabel, 1, 0);

    statsLayout->addWidget(new QLabel("En panne"), 0, 1);
    statsLayout->addWidget(panneLabel, 1, 1);

    statsLayout->addWidget(new QLabel("En service"), 0, 2);
    statsLayout->addWidget(serviceLabel, 1, 2);

    statsLayout->addWidget(new QLabel("Alertes"), 0, 3);
    statsLayout->addWidget(alerteLabel, 1, 3);

    // ==============================
    // TABLE
    // ==============================

    tableEquipements = new QTableWidget();

    tableEquipements->setColumnCount(6);

    QStringList headers;

    headers << "ID"
            << "Nom"
            << "Type"
            << "État"
            << "Dernière maintenance"
            << "Localisation";

    tableEquipements->setHorizontalHeaderLabels(headers);

    tableEquipements->setSelectionBehavior(
        QAbstractItemView::SelectRows
        );

    tableEquipements->setSelectionMode(
        QAbstractItemView::SingleSelection
        );

    tableEquipements->setEditTriggers(
        QAbstractItemView::NoEditTriggers
        );

    tableEquipements->horizontalHeader()
        ->setSectionResizeMode(QHeaderView::Stretch);

    tableEquipements->setAlternatingRowColors(true);

    // ==============================
    // LAYOUT RECHERCHE
    // ==============================

    QHBoxLayout *searchLayout = new QHBoxLayout();

    searchLayout->addWidget(searchEdit);
    searchLayout->addWidget(etatCombo);
    searchLayout->addWidget(typeCombo);

    // ==============================
    // LAYOUT BOUTONS
    // ==============================

    QHBoxLayout *buttonsLayout = new QHBoxLayout();

    buttonsLayout->addWidget(btnAjouter);
    buttonsLayout->addWidget(btnModifier);
    buttonsLayout->addWidget(btnSupprimer);
    buttonsLayout->addWidget(btnExporter);

    // ==============================
    // NODE ARDUINO
    // ==============================

    QLabel *arduinoStatus =
        new QLabel("● HydroSync Node : Online");

    arduinoStatus->setObjectName("arduinoStatus");

    // ==============================
    // LAYOUT PRINCIPAL
    // ==============================

    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    mainLayout->addWidget(title);
    mainLayout->addWidget(subtitle);

    mainLayout->addLayout(statsLayout);

    mainLayout->addLayout(searchLayout);

    mainLayout->addLayout(buttonsLayout);

    mainLayout->addWidget(tableEquipements);

    mainLayout->addWidget(arduinoStatus);

    setLayout(mainLayout);
}
void EquipmentManagement::chargerDonneesTest()
{
    tableEquipements->setRowCount(5);

    QString data[5][6] =
        {
            {"E001", "Pompe principale", "Pompe",
             "En service", "01/10/2026", "Zone A"},

            {"E002", "Filtre 1", "Filtre",
             "Maintenance", "25/09/2026", "Zone B"},

            {"E003", "Lampe UV", "Lampe UV",
             "En service", "20/09/2026", "Zone C"},

            {"E004", "Vanne purge", "Vanne",
             "En panne", "15/09/2026", "Zone A"},

            {"E005", "Pompe secondaire", "Pompe",
             "En service", "30/09/2026", "Zone D"}
        };

    for (int i = 0; i < 5; i++)
    {
        for (int j = 0; j < 6; j++)
        {
            tableEquipements->setItem(
                i,
                j,
                new QTableWidgetItem(data[i][j])
                );
        }
    }
}


void EquipmentManagement::rechercherEquipement()
{
    QString text = searchEdit->text().toLower();

    for (int i = 0; i < tableEquipements->rowCount(); i++)
    {
        QString id =
            tableEquipements->item(i, 0)->text().toLower();

        QString type =
            tableEquipements->item(i, 2)->text().toLower();

        bool match =
            id.contains(text) ||
            type.contains(text);

        tableEquipements->setRowHidden(i, !match);
    }
}


void EquipmentManagement::filtrerEquipement()
{
    QString etat = etatCombo->currentText();
    QString type = typeCombo->currentText();

    for (int i = 0; i < tableEquipements->rowCount(); i++)
    {
        QString currentEtat =
            tableEquipements->item(i, 3)->text();

        QString currentType =
            tableEquipements->item(i, 2)->text();

        bool okEtat =
            (etat == "Tous" || currentEtat == etat);

        bool okType =
            (type == "Tous" || currentType == type);

        tableEquipements->setRowHidden(
            i,
            !(okEtat && okType)
            );
    }
}


void EquipmentManagement::ajouterEquipement()
{
    int row = tableEquipements->rowCount();

    tableEquipements->insertRow(row);

    tableEquipements->setItem(
        row, 0,
        new QTableWidgetItem(
            "E00" + QString::number(row + 1)
            )
        );

    tableEquipements->setItem(
        row, 1,
        new QTableWidgetItem("Nouvel équipement")
        );

    tableEquipements->setItem(
        row, 2,
        new QTableWidgetItem("Pompe")
        );

    tableEquipements->setItem(
        row, 3,
        new QTableWidgetItem("En service")
        );

    tableEquipements->setItem(
        row, 4,
        new QTableWidgetItem("05/10/2026")
        );

    tableEquipements->setItem(
        row, 5,
        new QTableWidgetItem("Zone A")
        );

    mettreAJourStatistiques();
}


void EquipmentManagement::modifierEquipement()
{
    int row = tableEquipements->currentRow();

    if (row < 0)
    {
        QMessageBox::warning(
            this,
            "Modification",
            "Veuillez sélectionner un équipement."
            );

        return;
    }

    tableEquipements->item(row, 1)
        ->setText("Équipement modifié");

    mettreAJourStatistiques();
}


void EquipmentManagement::supprimerEquipement()
{
    int row = tableEquipements->currentRow();

    if (row < 0)
    {
        QMessageBox::warning(
            this,
            "Suppression",
            "Veuillez sélectionner un équipement."
            );

        return;
    }

    QMessageBox::StandardButton reply =
        QMessageBox::question(
            this,
            "Confirmation",
            "Voulez-vous supprimer cet équipement ?"
            );

    if (reply == QMessageBox::Yes)
    {
        tableEquipements->removeRow(row);

        mettreAJourStatistiques();
    }
}


void EquipmentManagement::mettreAJourStatistiques()
{
    int total = tableEquipements->rowCount();
    int panne = 0;
    int service = 0;

    for (int i = 0; i < total; i++)
    {
        QString etat =
            tableEquipements->item(i, 3)->text();

        if (etat == "En panne")
            panne++;

        if (etat == "En service")
            service++;
    }

    totalLabel->setText(QString::number(total));
    panneLabel->setText(QString::number(panne));
    serviceLabel->setText(QString::number(service));

    alerteLabel->setText(QString::number(panne));
}


void EquipmentManagement::exporterEquipements()
{
    QString fileName =
        QFileDialog::getSaveFileName(
            this,
            "Exporter les équipements",
            "",
            "CSV (*.csv)"
            );

    if (fileName.isEmpty())
        return;

    QFile file(fileName);

    if (!file.open(QIODevice::WriteOnly |
                   QIODevice::Text))
        return;

    QTextStream out(&file);

    for (int j = 0;
         j < tableEquipements->columnCount();
         j++)
    {
        out << tableEquipements
                   ->horizontalHeaderItem(j)
                   ->text();

        if (j < tableEquipements->columnCount() - 1)
            out << ";";
    }

    out << "\n";

    for (int i = 0;
         i < tableEquipements->rowCount();
         i++)
    {
        for (int j = 0;
             j < tableEquipements->columnCount();
             j++)
        {
            out << tableEquipements
                       ->item(i, j)
                       ->text();

            if (j < tableEquipements->columnCount() - 1)
                out << ";";
        }

        out << "\n";
    }

    file.close();

    QMessageBox::information(
        this,
        "Export",
        "Export terminé avec succès."
        );
}


void EquipmentManagement::appliquerStyle()
{
    setStyleSheet(R"(
        QWidget {
            background-color: #F4F7F9;
            font-family: Inter;
            font-size: 14px;
        }

        QLabel#title {
            color: #244058;
            font-size: 28px;
            font-weight: bold;
        }

        QLabel#statValue {
            color: #0070AF;
            font-size: 24px;
            font-weight: bold;
        }

        QLineEdit, QComboBox {
            background: white;
            border: 1px solid #D0D7DE;
            border-radius: 8px;
            padding: 10px;
        }

        QPushButton {
            background-color: #0070AF;
            color: white;
            border: none;
            border-radius: 8px;
            padding: 10px 18px;
        }

        QPushButton:hover {
            background-color: #0D72B9;
        }

        QTableWidget {
            background: white;
            border-radius: 8px;
            gridline-color: #E5E7EB;
        }

        QHeaderView::section {
            background-color: #0070AF;
            color: white;
            padding: 10px;
            font-weight: bold;
        }

        QLabel#arduinoStatus {
            color: #2ECC71;
            font-weight: bold;
        }
    )");
}