#include <QApplication>
#include <QFile>
#include <QMessageBox>

#include "database.h"
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QCoreApplication::setOrganizationName(QStringLiteral("HydroSync"));
    QCoreApplication::setApplicationName(QStringLiteral("HydroSync"));
    app.setStyle(QStringLiteral("Fusion")); // rendu identique sur tous les systèmes (utile pour le QSS)

    // Design : toute l'apparence (couleurs, tailles, boutons) est dans resources/style.qss
    QFile styleFile(QStringLiteral(":/style.qss"));
    if (styleFile.open(QIODevice::ReadOnly | QIODevice::Text))
        app.setStyleSheet(QString::fromUtf8(styleFile.readAll()));

    // Base SQLite : créée dans le dossier de données de l'utilisateur au premier lancement.
    QString error;
    if (!Database::open(&error)) {
        QMessageBox::warning(nullptr, QStringLiteral("Base de données"),
            QStringLiteral("La base SQLite n'a pas pu être ouverte :\n%1\n\n"
                           "L'application continue SANS sauvegarde (données en mémoire uniquement).").arg(error));
    }

    MainWindow window;
    window.resize(1360, 820);
    window.show();
    return app.exec();
}
