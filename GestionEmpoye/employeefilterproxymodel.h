#pragma once
#include <QDate>
#include <QSet>
#include <QSortFilterProxyModel>

// Applique les filtres (service, statut, date, recherche texte) et le tri au-dessus d'EmployeeModel.
class EmployeeFilterProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    explicit EmployeeFilterProxyModel(QObject *parent = nullptr);

    void setServices(const QSet<QString> &services);   // vide = tous
    void setStatuses(const QSet<QString> &statuses);   // vide = tous
    void setDateRange(bool enabled, const QDate &from, const QDate &to);
    void setSearchText(const QString &text);

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;
    bool lessThan(const QModelIndex &left, const QModelIndex &right) const override;

private:
    QSet<QString> m_services;
    QSet<QString> m_statuses;
    bool  m_dateEnabled = false;
    QDate m_from;
    QDate m_to;
    QString m_search;
};
