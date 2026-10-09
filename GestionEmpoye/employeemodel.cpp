#include "employeemodel.h"
#include "database.h"

#include <QColor>

EmployeeModel::EmployeeModel(QObject *parent) : QAbstractTableModel(parent)
{
    m_employees = Database::loadAll();
}

int EmployeeModel::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : m_employees.size();
}

int EmployeeModel::columnCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : ColumnCount;
}

QVariant EmployeeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= m_employees.size())
        return {};
    const Employee &e = m_employees.at(index.row());

    if (role == Qt::DisplayRole) {
        switch (index.column()) {
        case ColId:      return e.id;
        case ColNom:     return e.nom;
        case ColPrenoms: return e.prenoms;
        case ColPoste:   return e.poste;
        case ColService: return e.service;
        case ColDate:    return e.dateEmbauche.toString(QStringLiteral("dd/MM/yyyy"));
        case ColStatut:  return e.statut;
        default:         return QString();
        }
    }
    if (role == Qt::UserRole) { // valeur utilisée pour le tri
        switch (index.column()) {
        case ColId:      return e.id;
        case ColNom:     return e.nom;
        case ColPrenoms: return e.prenoms;
        case ColPoste:   return e.poste;
        case ColService: return e.service;
        case ColDate:    return e.dateEmbauche;
        case ColStatut:  return e.statut;
        default:         return {};
        }
    }
    if (role == Qt::ForegroundRole && index.column() == ColStatut) {
        if (e.statut == QLatin1String("Actif"))   return QColor(0x1e, 0x7e, 0x34);
        if (e.statut == QLatin1String("En congé")) return QColor(0xc7, 0x7c, 0x0e);
        return QColor(0x6c, 0x75, 0x7d);
    }
    if (role == Qt::TextAlignmentRole)
        return int(Qt::AlignLeft | Qt::AlignVCenter);
    return {};
}

QVariant EmployeeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation != Qt::Horizontal || role != Qt::DisplayRole)
        return {};
    static const QStringList titles = { QStringLiteral("ID"), QStringLiteral("Nom"), QStringLiteral("Prénoms"),
                                        QStringLiteral("Poste"), QStringLiteral("Service"),
                                        QStringLiteral("Date d'embauche"), QStringLiteral("Statut"),
                                        QStringLiteral("Action") };
    return (section >= 0 && section < titles.size()) ? titles.at(section) : QVariant();
}

void EmployeeModel::reload()
{
    beginResetModel();
    m_employees = Database::loadAll();
    endResetModel();
    emit employeesChanged();
}

bool EmployeeModel::addEmployee(const Employee &e, QString *error)
{
    if (idExists(e.id)) {
        if (error) *error = QStringLiteral("Cet identifiant existe déjà.");
        return false;
    }
    if (!Database::insert(e, error))
        return false;
    beginInsertRows(QModelIndex(), m_employees.size(), m_employees.size());
    m_employees.append(e);
    endInsertRows();
    emit employeesChanged();
    return true;
}

bool EmployeeModel::updateEmployee(int row, const Employee &e, QString *error)
{
    if (row < 0 || row >= m_employees.size()) return false;
    if (idExists(e.id, row)) {
        if (error) *error = QStringLiteral("Cet identifiant existe déjà.");
        return false;
    }
    if (!Database::update(m_employees.at(row).id, e, error))
        return false;
    m_employees[row] = e;
    emit dataChanged(index(row, 0), index(row, ColumnCount - 1));
    emit employeesChanged();
    return true;
}

bool EmployeeModel::removeEmployee(int row, QString *error)
{
    if (row < 0 || row >= m_employees.size()) return false;
    if (!Database::remove(m_employees.at(row).id, error))
        return false;
    beginRemoveRows(QModelIndex(), row, row);
    m_employees.removeAt(row);
    endRemoveRows();
    emit employeesChanged();
    return true;
}

Employee EmployeeModel::employeeAt(int row) const
{
    return (row >= 0 && row < m_employees.size()) ? m_employees.at(row) : Employee();
}

bool EmployeeModel::idExists(int id, int ignoreRow) const
{
    for (int i = 0; i < m_employees.size(); ++i)
        if (i != ignoreRow && m_employees.at(i).id == id)
            return true;
    return false;
}

int EmployeeModel::nextId() const
{
    int maxId = 1000;
    for (const Employee &e : m_employees)
        maxId = qMax(maxId, e.id);
    return maxId + 1;
}
