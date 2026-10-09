#pragma once
#include <QMap>
#include <QWidget>

class ActionDelegate;
class EmployeeFilterProxyModel;
class EmployeeModel;
class QCheckBox;
class QComboBox;
class QDateEdit;
class QLabel;
class QRadioButton;
class QTableView;

// Page « Employé » (image 2) : tableau, filtres/tri, statistiques RH, CRUD, export PDF.
class EmployeePage : public QWidget
{
    Q_OBJECT
public:
    explicit EmployeePage(QWidget *parent = nullptr);

public slots:
    void setSearchText(const QString &text);   // recherche globale (barre supérieure)
    void addEmployee();
    void editSelected();
    void deleteSelected();
    void searchEmployee();
    void exportPdf();

signals:
    void searchTextChanged(const QString &text);

private slots:
    void applyFilters();
    void applySort();
    void refreshStats();

private:
    QWidget *buildFilterPanel();
    QWidget *buildTablePanel();
    QWidget *buildStatsPanel();
    QWidget *buildCrudPanel();
    void updateSortCombo();
    void editRow(int sourceRow);
    void deleteRow(int sourceRow);
    int  selectedSourceRow() const;
    void selectSourceRow(int sourceRow);

    EmployeeModel *m_model;
    EmployeeFilterProxyModel *m_proxy;
    QTableView *m_table = nullptr;
    QLabel *m_countLabel = nullptr;
    QString m_searchText;

    // filtres
    QRadioButton *m_radioA = nullptr;
    QRadioButton *m_radioB = nullptr;
    QComboBox *m_sortCombo = nullptr;
    QCheckBox *m_descCheck = nullptr;
    QCheckBox *m_allServices = nullptr;
    QMap<QString, QCheckBox *> m_serviceChecks;
    QMap<QString, QCheckBox *> m_statusChecks;
    QCheckBox *m_dateCheck = nullptr;
    QDateEdit *m_dateFrom = nullptr;
    QDateEdit *m_dateTo = nullptr;

    // statistiques
    QLabel *m_statActive = nullptr;
    QLabel *m_statNew = nullptr;
    QLabel *m_statLeft = nullptr;
    QLabel *m_statSeniority = nullptr;
};
