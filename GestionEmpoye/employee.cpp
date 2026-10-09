#include "employee.h"

bool Employee::isValid() const
{
    return id > 0
        && !nom.trimmed().isEmpty()
        && !prenoms.trimmed().isEmpty()
        && !poste.trimmed().isEmpty()
        && !service.trimmed().isEmpty()
        && dateEmbauche.isValid()
        && !statut.trimmed().isEmpty();
}

QStringList Employee::services()
{
    return { QStringLiteral("Développement"), QStringLiteral("DevOps"),
             QStringLiteral("Design"),        QStringLiteral("RH"),
             QStringLiteral("Marketing") };
}

QStringList Employee::statuts()
{
    return { QStringLiteral("Actif"), QStringLiteral("En congé"), QStringLiteral("Inactif") };
}
