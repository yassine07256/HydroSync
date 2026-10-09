#pragma once
#include <QDate>
#include <QString>
#include <QStringList>

// Représente un employé (une ligne du tableau / de la base de données).
struct Employee
{
    int     id = 0;
    QString nom;
    QString prenoms;
    QString poste;
    QString service;
    QDate   dateEmbauche;
    QString statut = QStringLiteral("Actif");

    // Vrai si tous les champs obligatoires sont renseignés.
    bool isValid() const;

    // Listes de valeurs autorisées (utilisées par le formulaire et les filtres).
    static QStringList services();
    static QStringList statuts();
};
