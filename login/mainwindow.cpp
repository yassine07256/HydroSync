#include "mainwindow.h"

#include <QAction>
#include <QFrame>
#include <QGraphicsDropShadowEffect>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QIcon>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPainter>
#include <QPushButton>
#include <QStackedWidget>
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent)
{
    setWindowTitle("HydroSync");
    resize(900, 680);
    setMinimumSize(520, 700);

    // 1. Charger le background (une seule fois)
    m_background.load(":/resources/images/background.png");

    // 2. La carte centrale (effet verre)
    QFrame *card = new QFrame;
    card->setObjectName("card");
    card->setFixedWidth(420);

    // Ombre légère sous la carte
    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(card);
    shadow->setBlurRadius(40);
    shadow->setOffset(0, 8);
    shadow->setColor(QColor(0, 0, 0, 70));
    card->setGraphicsEffect(shadow);

    // 3. Le QStackedWidget avec nos 2 pages
    stackedWidget = new QStackedWidget;
    stackedWidget->setObjectName("stackedWidget");
    stackedWidget->addWidget(createLoginPage());     // index 0
    stackedWidget->addWidget(createRegisterPage());  // index 1
    stackedWidget->setCurrentIndex(0);               // Login au démarrage

    // 4. Contenu de la carte : en-tête + pages
    QVBoxLayout *cardLayout = new QVBoxLayout(card);
    cardLayout->setContentsMargins(32, 28, 32, 24);
    cardLayout->setSpacing(14);
    cardLayout->addWidget(createHeader());
    cardLayout->addWidget(stackedWidget);

    // 5. Centrer la carte dans la fenêtre
    QGridLayout *mainLayout = new QGridLayout(this);
    mainLayout->addWidget(card, 0, 0, Qt::AlignCenter);
}

// ---------- Background ----------
void MainWindow::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    if (m_background.isNull())
        return;

    // Agrandir l'image pour COUVRIR toute la fenêtre sans la déformer
    QPixmap scaled = m_background.scaled(size(),
                                         Qt::KeepAspectRatioByExpanding,
                                         Qt::SmoothTransformation);
    // Centrer l'image (ce qui dépasse est simplement rogné)
    int x = (width() - scaled.width()) / 2;
    int y = (height() - scaled.height()) / 2;
    painter.drawPixmap(x, y, scaled);
}

// ---------- En-tête : logo + titre ----------
QWidget *MainWindow::createHeader()
{
    QWidget *header = new QWidget;
    QHBoxLayout *layout = new QHBoxLayout(header);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(12);

    QLabel *logo = new QLabel;
    QPixmap pix(":/resources/images/hydrosync_logo.png");
    logo->setPixmap(pix.scaled(52, 52, Qt::KeepAspectRatio, Qt::SmoothTransformation));

    QLabel *title = new QLabel("HydroSync");
    title->setObjectName("titleLabel");

    layout->addStretch();
    layout->addWidget(logo);
    layout->addWidget(title);
    layout->addStretch();
    return header;
}

// ---------- Petites fonctions utilitaires ----------
QLineEdit *MainWindow::addField(QVBoxLayout *layout, const QString &labelText,
                                const QString &iconPath, bool isPassword)
{
    QLabel *label = new QLabel(labelText);
    label->setObjectName("fieldLabel");

    QLineEdit *edit = new QLineEdit;
    edit->setMinimumHeight(44);

    // Icône à GAUCHE du champ
    edit->addAction(QIcon(iconPath), QLineEdit::LeadingPosition);

    if (isPassword) {
        edit->setEchoMode(QLineEdit::Password);   // affiche des points

        // Icône œil à DROITE : une action "cochable" (allumée / éteinte)
        QAction *eye = edit->addAction(QIcon(":/resources/images/eye.png"),
                                       QLineEdit::TrailingPosition);
        eye->setCheckable(true);
        connect(eye, &QAction::toggled, this, [edit](bool visible) {
            edit->setEchoMode(visible ? QLineEdit::Normal : QLineEdit::Password);
        });
    }

    layout->addWidget(label);
    layout->addWidget(edit);
    return edit;
}

QPushButton *MainWindow::createFaceIdButton()
{
    QPushButton *button = new QPushButton("Face ID");
    button->setObjectName("faceIdButton");
    button->setIcon(QIcon(":/resources/images/face_id.png"));
    button->setIconSize(QSize(28, 28));
    button->setMinimumHeight(44);
    button->setCursor(Qt::PointingHandCursor);
    connect(button, &QPushButton::clicked, this, &MainWindow::onFaceIdClicked);
    return button;
}

QPushButton *MainWindow::createLinkButton(const QString &text)
{
    QPushButton *button = new QPushButton(text);
    button->setObjectName("linkButton");
    button->setFlat(true);
    button->setCursor(Qt::PointingHandCursor);
    return button;
}

// ---------- Page 0 : Login ----------
QWidget *MainWindow::createLoginPage()
{
    QWidget *page = new QWidget;
    page->setObjectName("loginPage");

    QVBoxLayout *layout = new QVBoxLayout(page);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(8);

    QLabel *welcome = new QLabel("Bon retour parmi nous !");
    welcome->setObjectName("welcomeLabel");
    welcome->setAlignment(Qt::AlignCenter);
    layout->addWidget(welcome);

    loginIdEdit = addField(layout, "ID ou Email",
                           ":/resources/images/email.png", false);
    loginPasswordEdit = addField(layout, "Mot de passe",
                                 ":/resources/images/lock.png", true);

    // "Mot de passe oublié ?" aligné à droite
    QPushButton *forgot = createLinkButton("Mot de passe oublié ?");
    forgot->setObjectName("forgotButton");
    connect(forgot, &QPushButton::clicked, this, &MainWindow::onForgotPasswordClicked);
    QHBoxLayout *forgotLayout = new QHBoxLayout;
    forgotLayout->addStretch();
    forgotLayout->addWidget(forgot);
    layout->addLayout(forgotLayout);

    QPushButton *loginButton = new QPushButton("Log in");
    loginButton->setObjectName("primaryButton");
    loginButton->setMinimumHeight(46);
    loginButton->setCursor(Qt::PointingHandCursor);
    connect(loginButton, &QPushButton::clicked, this, &MainWindow::onLoginClicked);
    layout->addWidget(loginButton);

    layout->addWidget(createFaceIdButton());

    QPushButton *toRegister =
        createLinkButton(QStringLiteral("Pas encore de compte ? S'inscrire ➜"));
    connect(toRegister, &QPushButton::clicked, this, &MainWindow::showRegisterPage);
    layout->addWidget(toRegister);

    return page;
}

// ---------- Page 1 : Inscription ----------
QWidget *MainWindow::createRegisterPage()
{
    QWidget *page = new QWidget;
    page->setObjectName("registerPage");

    QVBoxLayout *layout = new QVBoxLayout(page);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(8);

    QLabel *welcome = new QLabel("Rejoignez l'équipe");
    welcome->setObjectName("welcomeLabel");
    welcome->setAlignment(Qt::AlignCenter);
    layout->addWidget(welcome);

    registerIdEdit = addField(layout, "ID ou Email",
                              ":/resources/images/email.png", false);
    registerPasswordEdit = addField(layout, "Mot de passe",
                                    ":/resources/images/lock.png", true);
    registerConfirmEdit = addField(layout, "Vérifier mot de passe",
                                   ":/resources/images/lock.png", true);

    QPushButton *registerButton = new QPushButton("S'inscrire");
    registerButton->setObjectName("primaryButton");
    registerButton->setMinimumHeight(46);
    registerButton->setCursor(Qt::PointingHandCursor);
    connect(registerButton, &QPushButton::clicked, this, &MainWindow::onRegisterClicked);
    layout->addSpacing(6);
    layout->addWidget(registerButton);

    layout->addWidget(createFaceIdButton());

    QPushButton *toLogin =
        createLinkButton(QStringLiteral("Déjà un compte ? Se connecter ➜"));
    connect(toLogin, &QPushButton::clicked, this, &MainWindow::showLoginPage);
    layout->addWidget(toLogin);

    return page;
}

// ---------- Navigation ----------
void MainWindow::showLoginPage()
{
    stackedWidget->setCurrentIndex(0);
}

void MainWindow::showRegisterPage()
{
    stackedWidget->setCurrentIndex(1);
}

// ---------- Validation Login ----------
void MainWindow::onLoginClicked()
{
    const QString id = loginIdEdit->text().trimmed();
    const QString password = loginPasswordEdit->text();

    if (id.isEmpty()) {
        QMessageBox::warning(this, "Connexion", "Veuillez saisir votre ID ou votre email.");
        loginIdEdit->setFocus();
        return;
    }
    if (password.isEmpty()) {
        QMessageBox::warning(this, "Connexion", "Veuillez saisir votre mot de passe.");
        loginPasswordEdit->setFocus();
        return;
    }

    // TEMPORAIRE : plus tard, vérification dans la base de données
    QMessageBox::information(this, "Connexion", "Connexion réussie !");
}

// ---------- Validation Inscription ----------
void MainWindow::onRegisterClicked()
{
    const QString id = registerIdEdit->text().trimmed();
    const QString password = registerPasswordEdit->text();
    const QString confirm = registerConfirmEdit->text();

    if (id.isEmpty()) {
        QMessageBox::warning(this, "Inscription", "Veuillez saisir votre ID ou votre email.");
        registerIdEdit->setFocus();
        return;
    }
    if (password.isEmpty()) {
        QMessageBox::warning(this, "Inscription", "Veuillez saisir un mot de passe.");
        registerPasswordEdit->setFocus();
        return;
    }
    if (confirm.isEmpty()) {
        QMessageBox::warning(this, "Inscription", "Veuillez vérifier votre mot de passe.");
        registerConfirmEdit->setFocus();
        return;
    }
    if (password != confirm) {
        QMessageBox::warning(this, "Inscription", "Les mots de passe ne correspondent pas.");
        registerConfirmEdit->setFocus();
        return;
    }

    // TEMPORAIRE : plus tard, enregistrement dans la base de données
    QMessageBox::information(this, "Inscription", "Compte créé avec succès !");
}

// ---------- Autres boutons ----------
void MainWindow::onForgotPasswordClicked()
{
    QMessageBox::information(this, "Mot de passe oublié",
                             "Fonctionnalité de récupération du mot de passe.");
}

void MainWindow::onFaceIdClicked()
{
    // Pas de vraie reconnaissance faciale pour l'instant
    QMessageBox::information(this, "Face ID",
                             "Authentification Face ID non disponible pour le moment.");
}
