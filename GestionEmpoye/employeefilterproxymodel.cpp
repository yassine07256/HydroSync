#include "employeefilterproxymodel.h"
#include "employeemodel.h"

EmployeeFilterProxyModel::EmployeeFilterProxyModel(QObject *parent) : QSortFilterProxyModel(parent) {}

void EmployeeFilterProxyModel::setServices(const QSet<QString> &s) { m_services = s; invalidateFilter(); }
void EmployeeFilterProxyModel::setStatuses(const QSet<QString> &s) { m_statuses = s; invalidateFilter(); }

void EmployeeFilterProxyModel::setDateRange(bool enabled, const QDate &from, const QDate &to)
{
    m_dateEnabled = enabled;
    m_from = from;
    m_to = to;
    invalidateFilter();
}

void EmployeeFilterProxyModel::setSearchText(const QString &text)
{
    m_search = text.trimmed();
    invalidateFilter();
}

bool EmployeeFilterProxyModel::filterAcceptsRow(int row, const QModelIndex &parent) const
{
    const auto *m = qobject_cast<const EmployeeModel *>(sourceModel());
    if (!m) return true;
    const Employee e = m->employeeAt(row);
    Q_UNUSED(parent)

    if (!m_services.isEmpty() && !m_services.contains(e.service)) return false;
    if (!m_statuses.isEmpty() && !m_statuses.contains(e.statut)) return false;
    if (m_dateEnabled && (e.dateEmbauche < m_from || e.dateEmbauche > m_to)) return false;

    if (!m_search.isEmpty()) {
        const bool found = QString::number(e.id).contains(m_search, Qt::CaseInsensitive)
                        || e.nom.contains(m_search, Qt::CaseInsensitive)
                        || e.prenoms.contains(m_search, Qt::CaseInsensitive)
                        || e.poste.contains(m_search, Qt::CaseInsensitive);
        if (!found) return false;
    }
    return true;
}

bool EmployeeFilterProxyModel::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
    if (left.column() == EmployeeModel::ColAction)
        return false;
    const QVariant l = sourceModel()->data(left, Qt::UserRole);
    const QVariant r = sourceModel()->data(right, Qt::UserRole);
    if (l.typeId() == QMetaType::Int)  return l.toInt() < r.toInt();
    if (l.typeId() == QMetaType::QDate) return l.toDate() < r.toDate();
    return QString::localeAwareCompare(l.toString(), r.toString()) < 0;
}
