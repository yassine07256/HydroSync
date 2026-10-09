#include "database.h"

#include <QDir>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QStandardPaths>
#include <QVariant>

namespace {
const QString kConnection = QStringLiteral("hydrosync_employees");
QString g_path;
bool g_open = false;

QSqlDatabase db() { return QSqlDatabase::database(kConnection, false); }

Employee make(int id, const char *nom, const char *prenoms, const char *poste,
              const char *service, int y, int m, int d, const char *statut)
{
    Employee e;
    e.id = id;
    e.nom = QString::fromUtf8(nom);
    e.prenoms = QString::fromUtf8(prenoms);
    e.poste = QString::fromUtf8(poste);
    e.service = QString::fromUtf8(service);
    e.dateEmbauche = QDate(y, m, d);
    e.statut = QString::fromUtf8(statut);
    return e;
}

QString lastError(const QSqlQuery &q) { return q.lastError().text(); }
} // namespace

bool Database::open(QString *error)
{
    // Dossier de données de l'utilisateur (pas besoin de droits administrateur).
    const QString dir = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
    QDir().mkpath(dir);
    g_path = QDir(dir).filePath(QStringLiteral("employees.sqlite"));

    if (!QSqlDatabase::isDriverAvailable(QStringLiteral("QSQLITE"))) {
        if (error) *error = QStringLiteral("Le pilote Qt SQLite (QSQLITE) est introuvable.");
        return false;
    }

    QSqlDatabase database = QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"), kConnection);
    database.setDatabaseName(g_path);
    if (!database.open()) {
        if (error) *error = database.lastError().text();
        return false;
    }

    QSqlQuery q(database);
    if (!q.exec(QStringLiteral(
            "CREATE TABLE IF NOT EXISTS employees ("
            " id INTEGER PRIMARY KEY,"
            " nom TEXT NOT NULL,"
            " prenoms TEXT NOT NULL,"
            " poste TEXT NOT NULL,"
            " service TEXT NOT NULL,"
            " date_embauche TEXT NOT NULL,"
            " statut TEXT NOT NULL)"))) {
        if (error) *error = lastError(q);
        return false;
    }

    g_open = true;
    if (q.exec(QStringLiteral("SELECT COUNT(*) FROM employees")) && q.next() && q.value(0).toInt() == 0)
        seedDemoData();
    return true;
}

bool Database::isOpen() { return g_open; }
QString Database::filePath() { return g_path; }

void Database::seedDemoData()
{
    // Identifiants uniques et intitulés de postes corrigés par rapport à la capture.
    const QList<Employee> demo = {
        make(1023, "Dubois", "Léo Marc", "Développeur Senior",  "Développement", 2021, 1, 12, "Actif"),
        make(1024, "Thomas", "Emma",     "Développeur Senior",  "Développement", 2021, 1,  2, "Actif"),
        make(1025, "Thomas", "Emma",     "Développeur Senior",  "Développement", 2021, 1, 16, "Actif"),
        make(1026, "Anni",   "Emma",     "Ingénieur Logiciel",  "Développement", 2021, 1,  4, "Actif"),
        make(1028, "Rook",   "Einoa",    "Ingénieur Système",   "Développement", 2021, 9,  4, "Actif"),
        make(1037, "Rook",   "Einoa",    "Développeur Junior",  "Développement", 2021, 4,  4, "Actif"),
        make(1038, "Toett",  "Touts",    "Développeur Senior",  "Développement", 2021, 4,  4, "Actif"),
    };
    db().transaction();
    for (const Employee &e : demo)
        insert(e);
    db().commit();
}

QList<Employee> Database::loadAll()
{
    QList<Employee> list;
    if (!g_open) return list;
    QSqlQuery q(db());
    if (!q.exec(QStringLiteral("SELECT id, nom, prenoms, poste, service, date_embauche, statut "
                               "FROM employees ORDER BY id")))
        return list;
    while (q.next()) {
        Employee e;
        e.id = q.value(0).toInt();
        e.nom = q.value(1).toString();
        e.prenoms = q.value(2).toString();
        e.poste = q.value(3).toString();
        e.service = q.value(4).toString();
        e.dateEmbauche = QDate::fromString(q.value(5).toString(), Qt::ISODate);
        e.statut = q.value(6).toString();
        list.append(e);
    }
    return list;
}

bool Database::insert(const Employee &e, QString *error)
{
    if (!g_open) return true; // mode mémoire seulement
    QSqlQuery q(db());
    q.prepare(QStringLiteral("INSERT INTO employees (id, nom, prenoms, poste, service, date_embauche, statut) "
                             "VALUES (:id, :nom, :prenoms, :poste, :service, :date, :statut)"));
    q.bindValue(QStringLiteral(":id"), e.id);
    q.bindValue(QStringLiteral(":nom"), e.nom);
    q.bindValue(QStringLiteral(":prenoms"), e.prenoms);
    q.bindValue(QStringLiteral(":poste"), e.poste);
    q.bindValue(QStringLiteral(":service"), e.service);
    q.bindValue(QStringLiteral(":date"), e.dateEmbauche.toString(Qt::ISODate));
    q.bindValue(QStringLiteral(":statut"), e.statut);
    if (!q.exec()) {
        if (error) *error = lastError(q);
        return false;
    }
    return true;
}

bool Database::update(int oldId, const Employee &e, QString *error)
{
    if (!g_open) return true;
    QSqlQuery q(db());
    q.prepare(QStringLiteral("UPDATE employees SET id=:id, nom=:nom, prenoms=:prenoms, poste=:poste, "
                             "service=:service, date_embauche=:date, statut=:statut WHERE id=:old"));
    q.bindValue(QStringLiteral(":id"), e.id);
    q.bindValue(QStringLiteral(":nom"), e.nom);
    q.bindValue(QStringLiteral(":prenoms"), e.prenoms);
    q.bindValue(QStringLiteral(":poste"), e.poste);
    q.bindValue(QStringLiteral(":service"), e.service);
    q.bindValue(QStringLiteral(":date"), e.dateEmbauche.toString(Qt::ISODate));
    q.bindValue(QStringLiteral(":statut"), e.statut);
    q.bindValue(QStringLiteral(":old"), oldId);
    if (!q.exec()) {
        if (error) *error = lastError(q);
        return false;
    }
    return true;
}

bool Database::remove(int id, QString *error)
{
    if (!g_open) return true;
    QSqlQuery q(db());
    q.prepare(QStringLiteral("DELETE FROM employees WHERE id=:id"));
    q.bindValue(QStringLiteral(":id"), id);
    if (!q.exec()) {
        if (error) *error = lastError(q);
        return false;
    }
    return true;
}
