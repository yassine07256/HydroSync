#pragma once
#include <QAbstractTableModel>
#include <QList>
#include "employee.h"

// Modèle de données : fait le lien entre la liste d'employés (et la base SQLite) et le QTableView.
class EmployeeModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    enum Column { ColId, ColNom, ColPrenoms, ColPoste, ColService, ColDate, ColStatut, ColAction, ColumnCount };

    explicit EmployeeModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    void reload();
    bool addEmployee(const Employee &e, QString *error = nullptr);
    bool updateEmployee(int row, const Employee &e, QString *error = nullptr);
    bool removeEmployee(int row, QString *error = nullptr);

    Employee employeeAt(int row) const;
    bool idExists(int id, int ignoreRow = -1) const;
    int nextId() const;
    const QList<Employee> &employees() const { return m_employees; }

signals:
    void employeesChanged();

private:
    QList<Employee> m_employees;
};
