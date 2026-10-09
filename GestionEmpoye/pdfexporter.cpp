#include "pdfexporter.h"
#include "employeemodel.h"

#include <QAbstractItemModel>
#include <QDateTime>
#include <QMarginsF>
#include <QPageLayout>
#include <QPageSize>
#include <QPainter>
#include <QPdfWriter>

bool PdfExporter::exportModel(const QAbstractItemModel *model, const QString &path,
                              const QString &title, QString *error)
{
    QPdfWriter writer(path);
    writer.setPageSize(QPageSize(QPageSize::A4));
    writer.setPageOrientation(QPageLayout::Landscape);
    writer.setPageMargins(QMarginsF(12, 12, 12, 12), QPageLayout::Millimeter);
    writer.setResolution(96);
    writer.setTitle(title);

    QPainter p;
    if (!p.begin(&writer)) {
        if (error) *error = QStringLiteral("Impossible de créer le fichier : ") + path;
        return false;
    }

    const int cols = EmployeeModel::ColAction;   // toutes les colonnes sauf « Action »
    const QList<double> ratio = { 0.07, 0.14, 0.15, 0.22, 0.17, 0.15, 0.10 };
    const int pageW = writer.width();
    const int pageH = writer.height();

    QFont base(QStringLiteral("Arial"), 9);
    QFont bold = base; bold.setBold(true);
    QFont titleFont(QStringLiteral("Arial"), 16, QFont::Bold);

    p.setFont(base);
    const int rowH = p.fontMetrics().height() + 14;
    const int footerH = 30;

    QList<int> xs; int x = 0;
    for (int c = 0; c < cols; ++c) { xs << x; x += int(pageW * ratio.at(c)); }

    int y = 0;
    int pageNo = 1;

    auto drawHeader = [&]() {
        p.setFont(bold);
        p.fillRect(0, y, pageW, rowH, QColor(0xd9, 0xe4, 0xf0));
        p.setPen(Qt::black);
        for (int c = 0; c < cols; ++c)
            p.drawText(QRect(xs[c] + 6, y, int(pageW * ratio.at(c)) - 8, rowH), Qt::AlignVCenter | Qt::AlignLeft,
                       model->headerData(c, Qt::Horizontal).toString());
        y += rowH;
        p.setFont(base);
    };
    auto drawFooter = [&]() {
        p.setFont(base);
        p.setPen(QColor(0x66, 0x66, 0x66));
        p.drawText(QRect(0, pageH - footerH, pageW, footerH), Qt::AlignCenter,
                   QStringLiteral("HydroSync — Page %1").arg(pageNo));
    };

    p.setFont(titleFont);
    p.setPen(QColor(0x1f, 0x6f, 0xae));
    p.drawText(QRect(0, y, pageW, 44), Qt::AlignVCenter | Qt::AlignLeft, title);
    y += 44;
    p.setFont(base);
    p.setPen(QColor(0x44, 0x44, 0x44));
    p.drawText(QRect(0, y, pageW, 24), Qt::AlignVCenter | Qt::AlignLeft,
               QStringLiteral("Exporté le %1 — %2 employé(s)")
                   .arg(QDateTime::currentDateTime().toString(QStringLiteral("dd/MM/yyyy HH:mm")))
                   .arg(model->rowCount()));
    y += 36;
    drawHeader();

    for (int r = 0; r < model->rowCount(); ++r) {
        if (y + rowH > pageH - footerH - 6) {
            drawFooter();
            writer.newPage();
            ++pageNo;
            y = 0;
            drawHeader();
        }
        if (r % 2 == 1) p.fillRect(0, y, pageW, rowH, QColor(0xf3, 0xf6, 0xfa));
        p.setPen(Qt::black);
        for (int c = 0; c < cols; ++c)
            p.drawText(QRect(xs[c] + 6, y, int(pageW * ratio.at(c)) - 8, rowH), Qt::AlignVCenter | Qt::AlignLeft,
                       model->index(r, c).data(Qt::DisplayRole).toString());
        p.setPen(QColor(0xcc, 0xd3, 0xdb));
        p.drawLine(0, y + rowH, pageW, y + rowH);
        y += rowH;
    }
    drawFooter();
    p.end();
    return true;
}
