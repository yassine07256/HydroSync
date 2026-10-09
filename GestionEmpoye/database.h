#pragma once
#include <QList>
#include <QString>
#include "employee.h"

// Accès SQLite (Qt SQL). Toutes les fonctions sont statiques : une seule base pour l'application.
class Database
{
public:
    // Ouvre (et crée si besoin) la base. Insère les données de démonstration si elle est vide.
    static bool open(QString *error = nullptr);
    static bool isOpen();
    static QString filePath();

    static QList<Employee> loadAll();
    static bool insert(const Employee &e, QString *error = nullptr);
    static bool update(int oldId, const Employee &e, QString *error = nullptr);
    static bool remove(int id, QString *error = nullptr);

private:
    static void seedDemoData();
};
