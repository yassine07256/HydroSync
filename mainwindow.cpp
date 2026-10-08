#include "mainwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QPushButton>
#include <QLineEdit>
#include <QTableView>
#include <QLabel>
#include <QComboBox>
#include <QPixmap>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    // Application de la charte graphique globale
    this->setStyleSheet(
        "QMainWindow { background-color: #F4F7F9; }"
        "QGroupBox { font-weight: bold; color: #244058; border: 1px solid #0070AF; border-radius: 6px; margin-top: 10px; background-color: #FFFFFF; padding-top: 15px; }"
        "QGroupBox::title { subcontrol-origin: margin; left: 10px; padding: 0 5px; }"
        "QLabel { color: #244058; font-weight: bold; }"
        "QPushButton { background-color: #0070AF; color: white; border-radius: 4px; padding: 6px 12px; font-weight: bold; }"
        "QPushButton:hover { background-color: #00B4D3; }"
        "QLineEdit { background-color: #FFFFFF; border: 1px solid #BDC3C7; border-radius: 4px; padding: 4px; color: #2C3E50; }"
        "QComboBox { background-color: #FFFFFF; border: 1px solid #BDC3C7; border-radius: 4px; padding: 4px; color: #2C3E50; selection-background-color: #0070AF; selection-color: white; }"
        "QComboBox QAbstractItemView { background-color: #FFFFFF; color: #2C3E50; selection-background-color: #0070AF; selection-color: white; }"
        "QTableView { background-color: #FFFFFF; alternate-background-color: #F4F7F9; selection-background-color: #0070AF; selection-color: white; border: 1px solid #BDC3C7; }"
        );

    // 1. Widget central
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    // Layout principal vertical de la fenêtre
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // ==========================================
    // 2. HEADER SUPÉRIEUR (Bleu uni : Logo + Recherche + Utilisateur)
    // ==========================================
    QWidget *headerWidget = new QWidget(this);
    headerWidget->setFixedHeight(65);
    headerWidget->setStyleSheet("background-color: #0070AF;");
    QHBoxLayout *headerLayout = new QHBoxLayout(headerWidget);
    headerLayout->setContentsMargins(15, 0, 20, 0);

    // Intégration du Logo à gauche
    QLabel *lblLogoIcon = new QLabel(this);
    QPixmap pixmap("C:/qt 2a18/hydrosync_articles/logo_hydrosync.png");
    lblLogoIcon->setPixmap(pixmap.scaled(35, 35, Qt::KeepAspectRatio, Qt::SmoothTransformation));

    QLabel *lblLogoText = new QLabel("HydroSync", this);
    lblLogoText->setStyleSheet("color: white; font-size: 20px; font-weight: bold; margin-right: 30px;");

    headerLayout->addWidget(lblLogoIcon);
    headerLayout->addWidget(lblLogoText);

    // Barre de recherche au centre
    QLineEdit *searchBar = new QLineEdit(this);
    searchBar->setPlaceholderText("🔍 Rechercher un article par ID, Nom ou Contenance...");
    searchBar->setStyleSheet("background-color: #FFFFFF; border-radius: 15px; padding: 6px 15px; color: #2C3E50; font-size: 13px;");
    searchBar->setFixedWidth(400);

    // Utilisateur à droite
    QLabel *lblUser = new QLabel("👤 Yassine S. (Gestionnaire)", this);
    lblUser->setStyleSheet("color: white; font-weight: bold;");

    headerLayout->addWidget(searchBar);
    headerLayout->addStretch();
    headerLayout->addWidget(lblUser);

    mainLayout->addWidget(headerWidget);

    // ==========================================
    // 3. ZONE DE TRAVAIL : Gestion des Articles
    // ==========================================
    QWidget *workArea = new QWidget(this);
    QVBoxLayout *workLayout = new QVBoxLayout(workArea);
    workLayout->setContentsMargins(15, 15, 15, 15);

    // Ligne de titre et tri global sous le header
    QHBoxLayout *actionHeaderLayout = new QHBoxLayout();
    QLabel *pageTitle = new QLabel("Module : Gestion des Articles & Traçabilité", this);
    pageTitle->setStyleSheet("font-size: 16px; color: #244058; font-weight: bold;");

    QComboBox *sortCombo = new QComboBox(this);
    sortCombo->addItem("Trier par Défaut");
    sortCombo->addItem("Trier par Prix");
    sortCombo->addItem("Trier par Contenance");
    sortCombo->setFixedWidth(180);

    actionHeaderLayout->addWidget(pageTitle);
    actionHeaderLayout->addStretch();
    actionHeaderLayout->addWidget(sortCombo);
    workLayout->addLayout(actionHeaderLayout);

    // Division Centrale (Panneau Gauche + Panneau Droite)
    QHBoxLayout *centerLayout = new QHBoxLayout();

    // A. Panneau de gauche (Formulaire CRUD + Code-barres)
    QVBoxLayout *leftPanel = new QVBoxLayout();

    QGroupBox *formGroup = new QGroupBox("Détails de l'Article", this);
    QVBoxLayout *formLayout = new QVBoxLayout(formGroup);

    formLayout->addWidget(new QLabel("Nom de l'article :"));
    QLineEdit *txtNom = new QLineEdit(this);
    txtNom->setPlaceholderText("Ex: Eau Minérale");
    formLayout->addWidget(txtNom);

    formLayout->addWidget(new QLabel("Contenance :"));
    QComboBox *comboContenance = new QComboBox(this);
    comboContenance->addItems({"0.5 L", "1.0 L", "1.5 L", "5.0 L", "15.0 L"});
    formLayout->addWidget(comboContenance);

    formLayout->addWidget(new QLabel("Prix Unitaire (TND) :"));
    QLineEdit *txtPrix = new QLineEdit(this);
    txtPrix->setPlaceholderText("Ex: 1.200");
    formLayout->addWidget(txtPrix);

    // Boutons CRUD
    QPushButton *btnAjouter = new QPushButton("Ajouter", this);
    btnAjouter->setStyleSheet("QPushButton { background-color: #2ECC71; color: white; font-weight: bold; border-radius: 4px; padding: 6px; } QPushButton:hover { background-color: #27ae60; }");

    QPushButton *btnModifier = new QPushButton("Modifier", this);
    btnModifier->setStyleSheet("QPushButton { background-color: #0070AF; color: white; font-weight: bold; border-radius: 4px; padding: 6px; } QPushButton:hover { background-color: #005a8d; }");

    QPushButton *btnSupprimer = new QPushButton("Supprimer", this);
    btnSupprimer->setStyleSheet("QPushButton { background-color: #DC3545; color: white; font-weight: bold; border-radius: 4px; padding: 6px; } QPushButton:hover { background-color: #bd2130; }");

    formLayout->addWidget(btnAjouter);
    formLayout->addWidget(btnModifier);
    formLayout->addWidget(btnSupprimer);
    leftPanel->addWidget(formGroup);

    // Bloc Métier Innovant : Code-barres
    QGroupBox *innovGroup = new QGroupBox("Métier Innovant : Code-barres", this);
    QVBoxLayout *innovLayout = new QVBoxLayout(innovGroup);
    QPushButton *btnBarcode = new QPushButton("Générer Code-barres", this);
    QLabel *lblBarcode = new QLabel("[Aperçu du Code-barres]", this);
    lblBarcode->setAlignment(Qt::AlignCenter);
    innovLayout->addWidget(btnBarcode);
    innovLayout->addWidget(lblBarcode);
    leftPanel->addWidget(innovGroup);

    centerLayout->addLayout(leftPanel, 1);

    // B. Panneau de droite (Tableau + Calculateur de Rentabilité)
    QVBoxLayout *rightPanel = new QVBoxLayout();
    QTableView *tableView = new QTableView(this);
    rightPanel->addWidget(tableView, 3);

    QGroupBox *rentGroup = new QGroupBox("Calculateur de Rentabilité & Marge", this);
    QVBoxLayout *rentLayout = new QVBoxLayout(rentGroup);
    QLabel *lblCalculRentabilite = new QLabel("Prix au litre : - TND / Litre", this);
    rentLayout->addWidget(lblCalculRentabilite);
    rightPanel->addWidget(rentGroup, 1);

    centerLayout->addLayout(rightPanel, 2);
    workLayout->addLayout(centerLayout);

    // --- C. ZONE BASSE : Boutons d'exportation ---
    QHBoxLayout *footerLayout = new QHBoxLayout();
    QPushButton *btnPdf = new QPushButton("📄 Exporter PDF", this);
    QPushButton *btnExcel = new QPushButton("📊 Exporter Excel", this);
    QPushButton *btnStats = new QPushButton("📈 Statistiques (QtCharts)", this);

    footerLayout->addWidget(btnPdf);
    footerLayout->addWidget(btnExcel);
    footerLayout->addWidget(btnStats);
    workLayout->addLayout(footerLayout);

    mainLayout->addWidget(workArea);

    // Paramètres de la fenêtre
    setWindowTitle("Titan: HydroSync - Gestion des Articles");
    resize(1200, 750);
}

MainWindow::~MainWindow()
{
}