#ifndef CSTYPESDELEGATE_H
#define CSTYPESDELEGATE_H

#include <QStyledItemDelegate>

class CsTypesDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    CsTypesDelegate(QObject *parent = nullptr);
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const;
};

#endif // CSTYPESDELEGATE_H
