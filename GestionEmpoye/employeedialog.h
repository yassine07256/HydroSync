#pragma once
#include <QDialog>
#include <functional>
#include "employee.h"

class QComboBox;
class QDateEdit;
class QLineEdit;
class QSpinBox;

// Formulaire d'ajout / modification d'un employé.
class EmployeeDialog : public QDialog
{
    Q_OBJECT
public:
    // idExists : fonction fournie par l'appelant pour vérifier l'unicité de l'ID.
    EmployeeDialog(const QString &title, const Employee &initial, bool idEditable,
                   std::function<bool(int)> idExists, QWidget *parent = nullptr);

    Employee employee() const;

protected:
    void accept() override;

private:
    QSpinBox  *m_id;
    QLineEdit *m_nom;
    QLineEdit *m_prenoms;
    QLineEdit *m_poste;
    QComboBox *m_service;
    QDateEdit *m_date;
    QComboBox *m_statut;
    std::function<bool(int)> m_idExists;
    bool m_idEditable;
};
