#include "cstypesmodel.h"

CsTypesModel::CsTypesModel(QObject *parent):
    QAbstractListModel(parent)
{

}

int CsTypesModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return items.size();
}

QVariant CsTypesModel::data(const QModelIndex &index, int role) const
{
    if(role == Qt::DecorationRole)
        return items[index.row()].strIcon;
    else if(role == TypeRole)
        return items[index.row()].strType;
    else if(role == DescriptionRole)
        return items[index.row()].strDescription;
    else
        return QVariant();
}

bool CsTypesModel::setData(int iRow, const QVariant &value, int role)
{
    return setData(index(iRow), value, role);
}

bool CsTypesModel::insertRows(int row, int count, const QModelIndex &parent)
{
    if(row < 0 || (row > items.size() && items.size()!= 0))
        return false;
    beginInsertRows(parent,row, row+count-1);
    for(int i=row; i<row+count; i++)
    {
        items.insert(i, CsTypesItem());
    }
    endInsertRows();
    return true;
}

bool CsTypesModel::removeRows(int row, int count, const QModelIndex &parent)
{
    if(row < 0 || row+count > items.size())
        return false;
    beginRemoveRows(parent,row, row+count-1);
    for(int i=0; i<count; i++)
    {
        items.removeAt(row);
    }
    endRemoveRows();
    return true;
}

bool CsTypesModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if(index.row() >= items.size() || index.row() < 0)
        return QAbstractListModel::setData(index,value,role);

    if(role == Qt::DecorationRole )
        items[index.row()].strIcon = value.toString();
    else if(role == TypeRole)
        items[index.row()].strType = value.toString();
    else if(role == DescriptionRole)
        items[index.row()].strDescription = value.toString();
    else
        return QAbstractListModel::setData(index,value,role);

    emit dataChanged(index, index, {role});
    return true;
}
