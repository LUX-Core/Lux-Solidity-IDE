#ifndef CsTypesMODEL_H
#define CsTypesMODEL_H

#include <QAbstractListModel>

struct CsTypesItem
{
    CsTypesItem(QString strIcon,
                   QString strType,
                   QString strDescription):
                   strIcon(strIcon),
                   strType(strType),
                   strDescription(strDescription)
    {}
    CsTypesItem():
                   strIcon(),
                   strType(),
                   strDescription()
    {}
    QString strIcon;
    QString strType;
    QString strDescription;
};

class CsTypesModel : public QAbstractListModel
{
    Q_OBJECT

public:
    CsTypesModel(QObject *parent = nullptr);

    enum DataRole{
        TypeRole = Qt::UserRole + 1,
        DescriptionRole = Qt::UserRole + 2
    };

    //override functions
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;

    bool setData(int iRow, const QVariant &value, int role = Qt::EditRole);
private:
    QList<CsTypesItem> items;
};

#endif // CsTypesMODEL_H
