#pragma once
#include <QIcon>
#include <QStyledItemDelegate>

// Dessine les icônes « crayon » et « corbeille » dans la colonne Action et détecte les clics.
class ActionDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit ActionDelegate(QObject *parent = nullptr);

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option,
                     const QModelIndex &index) override;

signals:
    void editClicked(const QModelIndex &index);
    void deleteClicked(const QModelIndex &index);

private:
    QRect editRect(const QRect &cell) const;
    QRect deleteRect(const QRect &cell) const;

    QIcon m_edit;
    QIcon m_trash;
};
