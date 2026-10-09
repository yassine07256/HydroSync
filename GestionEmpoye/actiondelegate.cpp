#include "actiondelegate.h"

#include <QMouseEvent>
#include <QPainter>

namespace { constexpr int kIcon = 18; constexpr int kGap = 10; constexpr int kMargin = 8; }

ActionDelegate::ActionDelegate(QObject *parent)
    : QStyledItemDelegate(parent),
      m_edit(QStringLiteral(":/images/edit.svg")),
      m_trash(QStringLiteral(":/images/trash.svg"))
{
}

QRect ActionDelegate::deleteRect(const QRect &cell) const
{
    return QRect(cell.right() - kMargin - kIcon, cell.center().y() - kIcon / 2, kIcon, kIcon);
}

QRect ActionDelegate::editRect(const QRect &cell) const
{
    return deleteRect(cell).translated(-(kIcon + kGap), 0);
}

void ActionDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyledItemDelegate::paint(painter, option, index); // fond de sélection
    m_edit.paint(painter, editRect(option.rect));
    m_trash.paint(painter, deleteRect(option.rect));
}

QSize ActionDelegate::sizeHint(const QStyleOptionViewItem &, const QModelIndex &) const
{
    return QSize(2 * kIcon + kGap + 2 * kMargin, 30);
}

bool ActionDelegate::editorEvent(QEvent *event, QAbstractItemModel *, const QStyleOptionViewItem &option,
                                 const QModelIndex &index)
{
    if (event->type() == QEvent::MouseButtonRelease) {
        auto *me = static_cast<QMouseEvent *>(event);
        if (me->button() == Qt::LeftButton) {
            const QPoint pos = me->position().toPoint();
            if (editRect(option.rect).adjusted(-3, -3, 3, 3).contains(pos)) { emit editClicked(index); return true; }
            if (deleteRect(option.rect).adjusted(-3, -3, 3, 3).contains(pos)) { emit deleteClicked(index); return true; }
        }
    }
    return false;
}
