#pragma once
#include <QString>

class QAbstractItemModel;

// Exporte les lignes d'un modèle (ex. le modèle filtré/trié) vers un PDF A4 paysage.
class PdfExporter
{
public:
    static bool exportModel(const QAbstractItemModel *model, const QString &path,
                            const QString &title, QString *error = nullptr);
};
