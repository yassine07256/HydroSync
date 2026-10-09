#include "mainwindow.h"

#include "backgroundwidget.h"
#include "employeepage.h"
#include "homepage.h"

#include <QAction>
#include <QApplication>
#include <QButtonGroup>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMenu>
#include <QMessageBox>
#include <QPushButton>
#include <QStackedWidget>
#include <QToolButton>
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    setWindowTitle(QStringLiteral("HydroSync — Gestion des Employés"));
    setMinimumSize(1100, 720);

    auto *central = new QWidget;
    auto *rootLayout = new QHBoxLayout(central);
    rootLayout->setContentsMargins(0, 0, 0, 0);
    rootLayout->setSpacing(0);
    rootLayout->addWidget(buildSidebar());

    auto *rightColumn = new QWidget;
    auto *rightLayout = new QVBoxLayout(rightColumn);
    rightLayout->setContentsMargins(0, 0, 0, 0);
    rightLayout->setSpacing(0);
    rightLayout->addWidget(buildTopBar());

    // Zone centrale : image de fond + pages empilées (transparentes).
    auto *background = new BackgroundWidget(QStringLiteral(":/images/background.png"));
    auto *bgLayout = new QVBoxLayout(background);
    bgLayout->setContentsMargins(0, 0, 0, 0);

    m_stack = new QStackedWidget;
    m_employeePage = new EmployeePage;
    m_stack->addWidget(new HomePage);          // 0 Accueil
    m_stack->addWidget(m_employeePage);        // 1 Employé
    m_stack->addWidget(new QWidget);           // 2 Planning     (page vide)
    m_stack->addWidget(new QWidget);           // 3 Performance  (page vide)
    m_stack->addWidget(new QWidget);           // 4 Paramètre    (page vide)
    bgLayout->addWidget(m_stack);

    rightLayout->addWidget(background, 1);
    rootLayout->addWidget(rightColumn, 1);
    setCentralWidget(central);

    // Recherche : la page Employé informe la barre supérieure quand la recherche change.
    connect(m_employeePage, &EmployeePage::searchTextChanged, this, [this](const QString &t) {
        m_search->blockSignals(true);
        m_search->setText(t);
        m_search->blockSignals(false);
    });

    goToPage(PageHome);
}

QWidget *MainWindow::buildSidebar()
{
    auto *sidebar = new QWidget;
    sidebar->setObjectName(QStringLiteral("sidebar"));
    sidebar->setFixedWidth(205);

    auto *lay = new QVBoxLayout(sidebar);
    lay->setContentsMargins(0, 8, 0, 14);
    lay->setSpacing(2);

    auto *logoRow = new QHBoxLayout;
    logoRow->setContentsMargins(8, 0, 8, 0);
    auto *logo = new QLabel;
    logo->setPixmap(QPixmap(QStringLiteral(":/images/hydrosync_logo.png")).scaled(56, 56, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    auto *name = new QLabel(QStringLiteral("HydroSync"));
    name->setObjectName(QStringLiteral("brandName"));
    logoRow->addWidget(logo);
    logoRow->addWidget(name, 1);
    lay->addLayout(logoRow);
    lay->addSpacing(14);

    struct Item { QString text; QString icon; };
    const QList<Item> items = {
        { QStringLiteral("Accueil"),     QStringLiteral(":/images/home.svg") },
        { QStringLiteral("Employé"),     QStringLiteral(":/images/employee.svg") },
        { QStringLiteral("Planning"),    QStringLiteral(":/images/planning.svg") },
        { QStringLiteral("Performance"), QStringLiteral(":/images/performance.svg") },
        { QStringLiteral("Paramètre"),   QStringLiteral(":/images/settings.svg") },
    };
    m_navGroup = new QButtonGroup(this);
    m_navGroup->setExclusive(true);
    for (int i = 0; i < items.size(); ++i) {
        auto *b = new QPushButton(QIcon(items[i].icon), QStringLiteral("  ") + items[i].text);
        b->setProperty("nav", true);
        b->setCheckable(true);
        b->setIconSize(QSize(26, 26));
        b->setCursor(Qt::PointingHandCursor);
        m_navGroup->addButton(b, i);
        lay->addWidget(b);
    }
    connect(m_navGroup, &QButtonGroup::idClicked, this, &MainWindow::goToPage);

    lay->addStretch(1); // pousse « déconnexion » en bas, même après redimensionnement

    auto *logout = new QPushButton(QStringLiteral("déconnexion"));
    logout->setObjectName(QStringLiteral("logoutButton"));
    logout->setCursor(Qt::PointingHandCursor);
    connect(logout, &QPushButton::clicked, this, &MainWindow::confirmLogout);
    auto *logoutRow = new QHBoxLayout;
    logoutRow->setContentsMargins(22, 0, 22, 0);
    logoutRow->addWidget(logout);
    lay->addLayout(logoutRow);
    return sidebar;
}

QWidget *MainWindow::buildTopBar()
{
    auto *bar = new QWidget;
    bar->setObjectName(QStringLiteral("topBar"));
    bar->setFixedHeight(66);

    auto *lay = new QHBoxLayout(bar);
    lay->setContentsMargins(20, 6, 22, 6);
    lay->setSpacing(12);
    lay->addStretch(1);

    m_search = new QLineEdit;
    m_search->setPlaceholderText(QStringLiteral("Rechercher"));
    m_search->setClearButtonEnabled(true);
    m_search->addAction(QIcon(QStringLiteral(":/images/search.svg")), QLineEdit::LeadingPosition);
    m_search->setFixedWidth(320);
    connect(m_search, &QLineEdit::textChanged, this, &MainWindow::onTopSearchChanged);
    lay->addWidget(m_search);
    lay->addStretch(1);

    // Cloche : affiche les notifications récentes.
    auto *bell = new QToolButton;
    bell->setIcon(QIcon(QStringLiteral(":/images/bell.svg")));
    bell->setIconSize(QSize(28, 28));
    bell->setPopupMode(QToolButton::InstantPopup);
    bell->setCursor(Qt::PointingHandCursor);
    auto *bellMenu = new QMenu(bell);
    for (const QString &n : HomePage::notifications())
        bellMenu->addAction(n.length() > 64 ? n.left(61) + QStringLiteral("…") : n)->setToolTip(n);
    bell->setMenu(bellMenu);
    lay->addWidget(bell);

    auto *avatar = new QLabel;
    avatar->setPixmap(QIcon(QStringLiteral(":/images/user.svg")).pixmap(34, 34));
    lay->addWidget(avatar);

    auto *who = new QVBoxLayout;
    who->setSpacing(0);
    auto *userName = new QLabel(QStringLiteral("H. Robert"));
    userName->setObjectName(QStringLiteral("userName"));
    auto *role = new QLabel(QStringLiteral("(Technicienne de maintenance)"));
    role->setObjectName(QStringLiteral("userRole"));
    role->setWordWrap(true);
    role->setFixedWidth(150);
    who->addWidget(userName);
    who->addWidget(role);
    lay->addLayout(who);

    // Petite flèche : menu de profil.
    auto *arrow = new QToolButton;
    arrow->setIcon(QIcon(QStringLiteral(":/images/chevron.svg")));
    arrow->setIconSize(QSize(12, 12));
    arrow->setPopupMode(QToolButton::InstantPopup);
    arrow->setCursor(Qt::PointingHandCursor);
    auto *menu = new QMenu(arrow);
    menu->addAction(QStringLiteral("Mon profil"), this, [this] {
        QMessageBox::information(this, QStringLiteral("Profil"),
                                 QStringLiteral("H. Robert\nTechnicienne de maintenance"));
    });
    menu->addAction(QStringLiteral("Paramètre"), this, [this] { goToPage(PageSettings); });
    menu->addSeparator();
    menu->addAction(QStringLiteral("Déconnexion"), this, &MainWindow::confirmLogout);
    arrow->setMenu(menu);
    lay->addWidget(arrow, 0, Qt::AlignTop);
    return bar;
}

void MainWindow::goToPage(int index)
{
    m_stack->setCurrentIndex(index);
    if (auto *b = m_navGroup->button(index))
        b->setChecked(true); // met en évidence le bouton de la page courante
}

void MainWindow::onTopSearchChanged(const QString &text)
{
    m_employeePage->setSearchText(text);
    if (!text.trimmed().isEmpty() && m_stack->currentIndex() != PageEmployee)
        goToPage(PageEmployee);
}

void MainWindow::confirmLogout()
{
    const auto answer = QMessageBox::question(
        this, QStringLiteral("Déconnexion"),
        QStringLiteral("Voulez-vous vraiment vous déconnecter et fermer HydroSync ?"),
        QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
    if (answer == QMessageBox::Yes)
        QApplication::quit();
}
