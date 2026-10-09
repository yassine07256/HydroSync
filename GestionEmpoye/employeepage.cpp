#include "employeepage.h"

#include "actiondelegate.h"
#include "database.h"
#include "employeedialog.h"
#include "employeefilterproxymodel.h"
#include "employeemodel.h"
#include "glasspanel.h"
#include "pdfexporter.h"

#include <QCheckBox>
#include <QComboBox>
#include <QDateEdit>
#include <QDesktopServices>
#include <QFileDialog>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QInputDialog>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QRadioButton>
#include <QSignalBlocker>
#include <QStandardPaths>
#include <QTableView>
#include <QTimer>
#include <QUrl>
#include <QVBoxLayout>

namespace {

// QTableView transparent : on force un repaint complet au défilement, sinon l'image de fond
// « glisse » avec les lignes (le défilement par copie de pixels ne convient pas aux fonds translucides).
class TransparentTableView : public QTableView
{
public:
    using QTableView::QTableView;
protected:
    void scrollContentsBy(int dx, int dy) override
    {
        QTableView::scrollContentsBy(dx, dy);
        viewport()->update();
    }
};

template <class W> void darkText(W *w)
{
    QPalette pal = w->palette();
    pal.setColor(QPalette::WindowText, QColor(0x1b, 0x2a, 0x3a));
    w->setPalette(pal);
}

QLabel *label(const QString &text, double pt, bool bold = false)
{
    auto *l = new QLabel(text);
    QFont f = l->font();
    f.setPointSizeF(pt);
    f.setBold(bold);
    l->setFont(f);
    return l;
}

QPushButton *crudButton(const QString &text, const QString &icon)
{
    auto *b = new QPushButton(QIcon(icon), text);
    b->setCursor(Qt::PointingHandCursor);
    b->setIconSize(QSize(16, 16));
    b->setFlat(true);
    b->setObjectName(QStringLiteral("crudButton"));
    return b;
}
} // namespace

EmployeePage::EmployeePage(QWidget *parent) : QWidget(parent)
{
    m_model = new EmployeeModel(this);
    m_proxy = new EmployeeFilterProxyModel(this);
    m_proxy->setSourceModel(m_model);
    m_proxy->setSortRole(Qt::UserRole);

    auto *root = new QHBoxLayout(this);
    root->setContentsMargins(22, 16, 22, 16);
    root->setSpacing(16);

    root->addWidget(buildFilterPanel(), 0);

    auto *right = new QVBoxLayout;
    right->setSpacing(14);
    right->addWidget(buildTablePanel(), 1);

    auto *bottom = new QHBoxLayout;
    bottom->setSpacing(14);
    bottom->addWidget(buildStatsPanel(), 3);
    bottom->addWidget(buildCrudPanel(), 2);
    right->addLayout(bottom, 0);
    root->addLayout(right, 1);

    connect(m_model, &EmployeeModel::employeesChanged, this, &EmployeePage::refreshStats);
    connect(m_model, &EmployeeModel::employeesChanged, this, &EmployeePage::applyFilters);

    updateSortCombo();
    applySort();
    applyFilters();
    refreshStats();
}

// ---------------------------------------------------------------- construction UI
QWidget *EmployeePage::buildFilterPanel()
{
    auto *panel = new GlassPanel(nullptr, 215);
    panel->setFixedWidth(200);
    auto *lay = new QVBoxLayout(panel);
    lay->setContentsMargins(12, 10, 12, 10);
    lay->setSpacing(3);

    lay->addWidget(label(QStringLiteral("Filtres et Tri Intégrés"), 11, true));

    m_radioA = new QRadioButton(QStringLiteral("ID, Nom, Poste"));
    m_radioB = new QRadioButton(QStringLiteral("Service, Date, Statut"));
    m_radioA->setChecked(true);
    for (auto *r : { m_radioA, m_radioB }) { darkText(r); lay->addWidget(r); }

    m_sortCombo = new QComboBox;
    m_descCheck = new QCheckBox(QStringLiteral("Ordre décroissant"));
    darkText(m_descCheck);
    lay->addWidget(label(QStringLiteral("Trier par :"), 9.5));
    lay->addWidget(m_sortCombo);
    lay->addWidget(m_descCheck);

    connect(m_radioA, &QRadioButton::toggled, this, [this] { updateSortCombo(); applySort(); });
    connect(m_sortCombo, &QComboBox::currentIndexChanged, this, &EmployeePage::applySort);
    connect(m_descCheck, &QCheckBox::toggled, this, &EmployeePage::applySort);

    // Services
    lay->addSpacing(4);
    lay->addWidget(label(QStringLiteral("Service :"), 10, true));
    m_allServices = new QCheckBox(QStringLiteral("Tous"));
    m_allServices->setChecked(true);
    darkText(m_allServices);
    lay->addWidget(m_allServices);
    for (const QString &s : Employee::services()) {
        auto *cb = new QCheckBox(s);
        darkText(cb);
        m_serviceChecks.insert(s, cb);
        lay->addWidget(cb);
        connect(cb, &QCheckBox::toggled, this, [this](bool on) {
            if (on) { QSignalBlocker b(m_allServices); m_allServices->setChecked(false); }
            else {
                bool any = false;
                for (auto *c : std::as_const(m_serviceChecks)) any = any || c->isChecked();
                if (!any) { QSignalBlocker b(m_allServices); m_allServices->setChecked(true); }
            }
            applyFilters();
        });
    }
    connect(m_allServices, &QCheckBox::toggled, this, [this](bool on) {
        if (on) {
            for (auto *c : std::as_const(m_serviceChecks)) { QSignalBlocker b(c); c->setChecked(false); }
        } else {
            QSignalBlocker b(m_allServices);
            m_allServices->setChecked(true); // « Tous » ne peut pas être décoché seul
        }
        applyFilters();
    });

    // Statuts
    lay->addSpacing(4);
    lay->addWidget(label(QStringLiteral("Statut :"), 10, true));
    for (const QString &s : Employee::statuts()) {
        auto *cb = new QCheckBox(s);
        darkText(cb);
        m_statusChecks.insert(s, cb);
        lay->addWidget(cb);
        connect(cb, &QCheckBox::toggled, this, &EmployeePage::applyFilters);
    }

    // Date d'embauche
    lay->addSpacing(4);
    m_dateCheck = new QCheckBox(QStringLiteral("Date d'embauche :"));
    darkText(m_dateCheck);
    { QFont f = m_dateCheck->font(); f.setBold(true); m_dateCheck->setFont(f); }
    lay->addWidget(m_dateCheck);
    m_dateFrom = new QDateEdit(QDate(2020, 1, 1));
    m_dateTo = new QDateEdit(QDate::currentDate());
    for (auto *d : { m_dateFrom, m_dateTo }) {
        d->setDisplayFormat(QStringLiteral("dd/MM/yyyy"));
        d->setCalendarPopup(true);
        d->setEnabled(false);
    }
    auto *fromRow = new QHBoxLayout; fromRow->addWidget(label(QStringLiteral("Du"), 9.5)); fromRow->addWidget(m_dateFrom);
    auto *toRow = new QHBoxLayout;   toRow->addWidget(label(QStringLiteral("Au"), 9.5));   toRow->addWidget(m_dateTo);
    lay->addLayout(fromRow);
    lay->addLayout(toRow);
    connect(m_dateCheck, &QCheckBox::toggled, this, [this](bool on) {
        m_dateFrom->setEnabled(on);
        m_dateTo->setEnabled(on);
        applyFilters();
    });
    connect(m_dateFrom, &QDateEdit::dateChanged, this, &EmployeePage::applyFilters);
    connect(m_dateTo, &QDateEdit::dateChanged, this, &EmployeePage::applyFilters);

    lay->addStretch(1);

    auto *reset = new QPushButton(QStringLiteral("Réinitialiser"));
    connect(reset, &QPushButton::clicked, this, [this] {
        for (auto *c : std::as_const(m_serviceChecks)) { QSignalBlocker b(c); c->setChecked(false); }
        for (auto *c : std::as_const(m_statusChecks)) { QSignalBlocker b(c); c->setChecked(false); }
        { QSignalBlocker b(m_allServices); m_allServices->setChecked(true); }
        { QSignalBlocker b(m_dateCheck); m_dateCheck->setChecked(false); }
        m_dateFrom->setEnabled(false);
        m_dateTo->setEnabled(false);
        setSearchText(QString());
        emit searchTextChanged(QString());
    });
    lay->addWidget(reset);
    return panel;
}

QWidget *EmployeePage::buildTablePanel()
{
    auto *panel = new GlassPanel(nullptr, 205);
    auto *lay = new QVBoxLayout(panel);
    lay->setContentsMargins(14, 10, 14, 10);

    auto *head = new QHBoxLayout;
    auto *titles = new QVBoxLayout;
    titles->setSpacing(0);
    titles->addWidget(label(QStringLiteral("Tableau de Bord des Employés"), 15));
    titles->addWidget(label(QStringLiteral("Liste des Employés"), 11));
    head->addLayout(titles);
    head->addStretch(1);
    m_countLabel = label(QString(), 9.5);
    head->addWidget(m_countLabel, 0, Qt::AlignBottom);
    lay->addLayout(head);

    m_table = new TransparentTableView;
    m_table->setModel(m_proxy);
    m_table->setSortingEnabled(true);
    m_table->setObjectName(QStringLiteral("employeeTable"));
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setSelectionMode(QAbstractItemView::SingleSelection);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->setShowGrid(false);
    m_table->setFrameShape(QFrame::NoFrame);
    m_table->setAlternatingRowColors(false);
    m_table->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    m_table->verticalHeader()->hide();
    m_table->verticalHeader()->setDefaultSectionSize(30);
    m_table->viewport()->setAutoFillBackground(false);
    m_table->setAutoFillBackground(false);

    auto *hh = m_table->horizontalHeader();
    hh->setHighlightSections(false);
    hh->setSectionResizeMode(QHeaderView::ResizeToContents);
    hh->setMinimumSectionSize(50);
    hh->setSectionResizeMode(EmployeeModel::ColPrenoms, QHeaderView::Stretch);
    hh->setSectionResizeMode(EmployeeModel::ColAction, QHeaderView::Fixed);
    m_table->setColumnWidth(EmployeeModel::ColAction, 80);
    hh->setStretchLastSection(false);

    auto *delegate = new ActionDelegate(this);
    m_table->setItemDelegateForColumn(EmployeeModel::ColAction, delegate);
    // Les opérations sont exécutées après le retour du gestionnaire de clic (QTimer 0) pour éviter
    // de modifier le modèle pendant que la vue traite encore l'événement souris.
    connect(delegate, &ActionDelegate::editClicked, this, [this](const QModelIndex &i) {
        const int row = m_proxy->mapToSource(i).row();
        selectSourceRow(row);
        QTimer::singleShot(0, this, [this, row] { editRow(row); });
    });
    connect(delegate, &ActionDelegate::deleteClicked, this, [this](const QModelIndex &i) {
        const int row = m_proxy->mapToSource(i).row();
        selectSourceRow(row);
        QTimer::singleShot(0, this, [this, row] { deleteRow(row); });
    });
    connect(m_table, &QTableView::doubleClicked, this, [this](const QModelIndex &i) {
        if (i.column() != EmployeeModel::ColAction) editRow(m_proxy->mapToSource(i).row());
    });

    lay->addWidget(m_table, 1);
    return panel;
}

QWidget *EmployeePage::buildStatsPanel()
{
    auto *panel = new GlassPanel(nullptr, 225);
    auto *lay = new QVBoxLayout(panel);
    lay->setContentsMargins(16, 10, 16, 12);

    auto *head = new QHBoxLayout;
    head->addWidget(label(QStringLiteral("Statistiques Clés RH"), 12, true));
    head->addStretch(1);
    auto *pdf = new QPushButton(QIcon(QStringLiteral(":/images/pdf.svg")), QStringLiteral("Exporter PDF"));
    pdf->setIcon(QIcon(QStringLiteral(":/images/pdf.svg")));
    pdf->setCursor(Qt::PointingHandCursor);
    pdf->setObjectName(QStringLiteral("exportPdfButton"));
    connect(pdf, &QPushButton::clicked, this, &EmployeePage::exportPdf);
    head->addWidget(pdf);
    auto *dl = new QLabel;
    dl->setPixmap(QIcon(QStringLiteral(":/images/download_dark.svg")).pixmap(16, 16));
    head->addWidget(dl);
    lay->addLayout(head);

    auto *grid = new QGridLayout;
    grid->setVerticalSpacing(4);
    auto addRow = [&](int row, const QString &caption, QLabel *&value) {
        grid->addWidget(label(caption, 10.5), row, 0);
        value = label(QStringLiteral("-"), 11.5, true);
        grid->addWidget(value, row, 1, Qt::AlignRight);
    };
    addRow(0, QStringLiteral("Total Employés Actifs :"), m_statActive);
    addRow(1, QStringLiteral("Nouveaux ce mois :"), m_statNew);
    addRow(2, QStringLiteral("Départs ce mois :"), m_statLeft);
    addRow(3, QStringLiteral("Ancienneté Moyenne :"), m_statSeniority);
    grid->setColumnStretch(0, 1);
    lay->addLayout(grid);
    return panel;
}

QWidget *EmployeePage::buildCrudPanel()
{
    auto *panel = new GlassPanel(nullptr, 225);
    auto *lay = new QVBoxLayout(panel);
    lay->setContentsMargins(14, 10, 14, 10);
    lay->setSpacing(0);
    lay->addWidget(label(QStringLiteral("CRUD Employés"), 12, true));

    struct Item { QString text; QString icon; void (EmployeePage::*slot)(); };
    const QList<Item> items = {
        { QStringLiteral("Ajouter un Nouvel Employé"),          QStringLiteral(":/images/plus.svg"),         &EmployeePage::addEmployee },
        { QStringLiteral("Modifier l'Employé Sélectionné"),     QStringLiteral(":/images/edit.svg"),         &EmployeePage::editSelected },
        { QStringLiteral("Supprimer l'Employé Sélectionné"),    QStringLiteral(":/images/trash.svg"),        &EmployeePage::deleteSelected },
        { QStringLiteral("Rechercher un Employé (Nom, ID...)"), QStringLiteral(":/images/search.svg"),       &EmployeePage::searchEmployee },
        { QStringLiteral("Exporter la Liste PDF"),              QStringLiteral(":/images/download_dark.svg"), &EmployeePage::exportPdf },
    };
    for (const Item &it : items) {
        auto *b = crudButton(it.text, it.icon);
        connect(b, &QPushButton::clicked, this, it.slot);
        lay->addWidget(b);
    }
    return panel;
}

// ---------------------------------------------------------------------- tri / filtres
void EmployeePage::updateSortCombo()
{
    QSignalBlocker b(m_sortCombo);
    m_sortCombo->clear();
    if (m_radioA->isChecked()) {
        m_sortCombo->addItem(QStringLiteral("ID"), int(EmployeeModel::ColId));
        m_sortCombo->addItem(QStringLiteral("Nom"), int(EmployeeModel::ColNom));
        m_sortCombo->addItem(QStringLiteral("Poste"), int(EmployeeModel::ColPoste));
    } else {
        m_sortCombo->addItem(QStringLiteral("Service"), int(EmployeeModel::ColService));
        m_sortCombo->addItem(QStringLiteral("Date d'embauche"), int(EmployeeModel::ColDate));
        m_sortCombo->addItem(QStringLiteral("Statut"), int(EmployeeModel::ColStatut));
    }
}

void EmployeePage::applySort()
{
    if (m_sortCombo->currentIndex() < 0) return;
    const int col = m_sortCombo->currentData().toInt();
    m_table->sortByColumn(col, m_descCheck->isChecked() ? Qt::DescendingOrder : Qt::AscendingOrder);
}

void EmployeePage::applyFilters()
{
    QSet<QString> services, statuses;
    if (!m_allServices->isChecked())
        for (auto it = m_serviceChecks.cbegin(); it != m_serviceChecks.cend(); ++it)
            if (it.value()->isChecked()) services.insert(it.key());
    for (auto it = m_statusChecks.cbegin(); it != m_statusChecks.cend(); ++it)
        if (it.value()->isChecked()) statuses.insert(it.key());

    m_proxy->setServices(services);
    m_proxy->setStatuses(statuses);
    m_proxy->setDateRange(m_dateCheck->isChecked(), m_dateFrom->date(), m_dateTo->date());
    m_proxy->setSearchText(m_searchText);
    m_countLabel->setText(QStringLiteral("%1 employé(s) affiché(s) sur %2")
                              .arg(m_proxy->rowCount()).arg(m_model->rowCount()));
}

void EmployeePage::setSearchText(const QString &text)
{
    m_searchText = text;
    applyFilters();
}

void EmployeePage::refreshStats()
{
    int active = 0, fresh = 0;
    double years = 0;
    const QDate today = QDate::currentDate();
    for (const Employee &e : m_model->employees()) {
        if (e.statut == QLatin1String("Actif")) ++active;
        if (e.dateEmbauche.year() == today.year() && e.dateEmbauche.month() == today.month()) ++fresh;
        years += e.dateEmbauche.daysTo(today) / 365.25;
    }
    const int n = m_model->employees().size();
    m_statActive->setText(QString::number(active));
    m_statNew->setText(QString::number(fresh));
    m_statLeft->setText(QStringLiteral("1"));
    m_statLeft->setToolTip(QStringLiteral("Valeur de démonstration : la base ne stocke pas de date de départ."));
    m_statSeniority->setText(n > 0 ? QStringLiteral("%1 ans").arg(years / n, 0, 'f', 1) : QStringLiteral("0 an"));
}

// ------------------------------------------------------------------------------ CRUD
int EmployeePage::selectedSourceRow() const
{
    const QModelIndexList sel = m_table->selectionModel()->selectedRows();
    return sel.isEmpty() ? -1 : m_proxy->mapToSource(sel.first()).row();
}

void EmployeePage::selectSourceRow(int sourceRow)
{
    const QModelIndex p = m_proxy->mapFromSource(m_model->index(sourceRow, 0));
    if (p.isValid()) {
        m_table->selectRow(p.row());
        m_table->scrollTo(p);
    }
}

void EmployeePage::addEmployee()
{
    Employee init;
    init.id = m_model->nextId();
    init.dateEmbauche = QDate::currentDate();
    EmployeeDialog dlg(QStringLiteral("Ajouter un nouvel employé"), init, true,
                       [this](int id) { return m_model->idExists(id); }, this);
    if (dlg.exec() != QDialog::Accepted) return;

    QString err;
    const Employee e = dlg.employee();
    if (!m_model->addEmployee(e, &err)) {
        QMessageBox::critical(this, QStringLiteral("Erreur"), QStringLiteral("Ajout impossible : ") + err);
        return;
    }
    // retrouver la ligne ajoutée (elle peut être cachée par les filtres actifs)
    for (int r = 0; r < m_model->rowCount(); ++r) {
        if (m_model->employeeAt(r).id == e.id) {
            if (m_proxy->mapFromSource(m_model->index(r, 0)).isValid())
                selectSourceRow(r);
            else
                QMessageBox::information(this, QStringLiteral("Employé ajouté"),
                    QStringLiteral("L'employé a été ajouté, mais il est masqué par les filtres ou la recherche actifs."));
            break;
        }
    }
}

void EmployeePage::editSelected()
{
    const int row = selectedSourceRow();
    if (row < 0) {
        QMessageBox::information(this, QStringLiteral("Aucune sélection"),
                                 QStringLiteral("Veuillez d'abord sélectionner un employé dans le tableau."));
        return;
    }
    editRow(row);
}

void EmployeePage::editRow(int sourceRow)
{
    if (sourceRow < 0 || sourceRow >= m_model->rowCount()) return;
    const Employee current = m_model->employeeAt(sourceRow);
    EmployeeDialog dlg(QStringLiteral("Modifier l'employé"), current, false,
                       [this, sourceRow](int id) { return m_model->idExists(id, sourceRow); }, this);
    if (dlg.exec() != QDialog::Accepted) return;
    QString err;
    if (!m_model->updateEmployee(sourceRow, dlg.employee(), &err))
        QMessageBox::critical(this, QStringLiteral("Erreur"), QStringLiteral("Modification impossible : ") + err);
}

void EmployeePage::deleteSelected()
{
    const int row = selectedSourceRow();
    if (row < 0) {
        QMessageBox::information(this, QStringLiteral("Aucune sélection"),
                                 QStringLiteral("Veuillez d'abord sélectionner un employé dans le tableau."));
        return;
    }
    deleteRow(row);
}

void EmployeePage::deleteRow(int sourceRow)
{
    if (sourceRow < 0 || sourceRow >= m_model->rowCount()) return;
    const Employee e = m_model->employeeAt(sourceRow);
    const auto answer = QMessageBox::question(
        this, QStringLiteral("Confirmer la suppression"),
        QStringLiteral("Voulez-vous vraiment supprimer l'employé %1 %2 (ID %3) ?").arg(e.nom, e.prenoms).arg(e.id),
        QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
    if (answer != QMessageBox::Yes) return;
    QString err;
    if (!m_model->removeEmployee(sourceRow, &err))
        QMessageBox::critical(this, QStringLiteral("Erreur"), QStringLiteral("Suppression impossible : ") + err);
}

void EmployeePage::searchEmployee()
{
    bool ok = false;
    const QString text = QInputDialog::getText(this, QStringLiteral("Rechercher un employé"),
                                               QStringLiteral("ID, nom, prénom ou poste :"),
                                               QLineEdit::Normal, m_searchText, &ok);
    if (!ok) return;
    setSearchText(text.trimmed());
    emit searchTextChanged(text.trimmed());
    if (m_proxy->rowCount() == 0)
        QMessageBox::information(this, QStringLiteral("Recherche"), QStringLiteral("Aucun employé ne correspond à « %1 ».").arg(text));
    else if (!text.trimmed().isEmpty())
        m_table->selectRow(0);
}

// -------------------------------------------------------------------------- export PDF
void EmployeePage::exportPdf()
{
    const QString defaultDir = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    QString path = QFileDialog::getSaveFileName(this, QStringLiteral("Exporter la liste des employés"),
                                                defaultDir + QStringLiteral("/Liste_Employes.pdf"),
                                                QStringLiteral("Fichiers PDF (*.pdf)"));
    if (path.isEmpty()) return;
    if (!path.endsWith(QStringLiteral(".pdf"), Qt::CaseInsensitive)) path += QStringLiteral(".pdf");

    QString err;
    if (!PdfExporter::exportModel(m_proxy, path, QStringLiteral("Liste des Employés — HydroSync"), &err)) {
        QMessageBox::critical(this, QStringLiteral("Export PDF"), QStringLiteral("Échec de l'export : ") + err);
        return;
    }
    QMessageBox box(QMessageBox::Information, QStringLiteral("Export PDF"),
                    QStringLiteral("Le fichier PDF a été créé :\n%1").arg(path), QMessageBox::Ok, this);
    QPushButton *open = box.addButton(QStringLiteral("Ouvrir le PDF"), QMessageBox::ActionRole);
    box.exec();
    if (box.clickedButton() == open)
        QDesktopServices::openUrl(QUrl::fromLocalFile(path));
}
