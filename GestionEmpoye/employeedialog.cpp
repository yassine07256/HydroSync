#include "employeedialog.h"

#include <QComboBox>
#include <QDateEdit>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QSpinBox>
#include <QVBoxLayout>

EmployeeDialog::EmployeeDialog(const QString &title, const Employee &initial, bool idEditable,
                               std::function<bool(int)> idExists, QWidget *parent)
    : QDialog(parent), m_idExists(std::move(idExists)), m_idEditable(idEditable)
{
    setWindowTitle(title);
    setModal(true);
    setMinimumWidth(380);

    m_id = new QSpinBox(this);
    m_id->setRange(1, 999999);
    m_id->setValue(initial.id > 0 ? initial.id : 1001);
    m_id->setEnabled(idEditable);

    m_nom = new QLineEdit(initial.nom, this);
    m_prenoms = new QLineEdit(initial.prenoms, this);
    m_poste = new QLineEdit(initial.poste, this);

    m_service = new QComboBox(this);
    m_service->addItems(Employee::services());
    if (!initial.service.isEmpty()) {
        if (m_service->findText(initial.service) < 0) m_service->addItem(initial.service);
        m_service->setCurrentText(initial.service);
    }

    m_date = new QDateEdit(initial.dateEmbauche.isValid() ? initial.dateEmbauche : QDate::currentDate(), this);
    m_date->setDisplayFormat(QStringLiteral("dd/MM/yyyy"));
    m_date->setCalendarPopup(true);
    m_date->setDateRange(QDate(1970, 1, 1), QDate::currentDate().addYears(1));

    m_statut = new QComboBox(this);
    m_statut->addItems(Employee::statuts());
    if (!initial.statut.isEmpty()) m_statut->setCurrentText(initial.statut);

    auto *form = new QFormLayout;
    form->addRow(QStringLiteral("ID *"), m_id);
    form->addRow(QStringLiteral("Nom *"), m_nom);
    form->addRow(QStringLiteral("Prénoms *"), m_prenoms);
    form->addRow(QStringLiteral("Poste *"), m_poste);
    form->addRow(QStringLiteral("Service *"), m_service);
    form->addRow(QStringLiteral("Date d'embauche *"), m_date);
    form->addRow(QStringLiteral("Statut *"), m_statut);

    auto *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    buttons->button(QDialogButtonBox::Ok)->setText(QStringLiteral("Valider"));
    buttons->button(QDialogButtonBox::Cancel)->setText(QStringLiteral("Annuler"));
    connect(buttons, &QDialogButtonBox::accepted, this, &EmployeeDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &EmployeeDialog::reject);

    auto *layout = new QVBoxLayout(this);
    layout->addLayout(form);
    layout->addWidget(buttons);
}

Employee EmployeeDialog::employee() const
{
    Employee e;
    e.id = m_id->value();
    e.nom = m_nom->text().trimmed();
    e.prenoms = m_prenoms->text().trimmed();
    e.poste = m_poste->text().trimmed();
    e.service = m_service->currentText();
    e.dateEmbauche = m_date->date();
    e.statut = m_statut->currentText();
    return e;
}

void EmployeeDialog::accept()
{
    const Employee e = employee();
    QString problem;
    if (e.nom.isEmpty())          problem = QStringLiteral("Le nom est obligatoire.");
    else if (e.prenoms.isEmpty()) problem = QStringLiteral("Les prénoms sont obligatoires.");
    else if (e.poste.isEmpty())   problem = QStringLiteral("Le poste est obligatoire.");
    else if (m_idEditable && m_idExists && m_idExists(e.id))
        problem = QStringLiteral("L'identifiant %1 existe déjà. Choisissez-en un autre.").arg(e.id);

    if (!problem.isEmpty()) {
        QMessageBox::warning(this, QStringLiteral("Champs invalides"), problem);
        return;
    }
    QDialog::accept();
}
