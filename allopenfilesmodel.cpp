#include "allopenfilesmodel.h"

#define defTmpFileToolTip QString("It is only tmp file. But you can store it in disk if you click \"Save as\".")


AllOpenFilesModel::AllOpenFilesModel(QObject *parent):
    QAbstractListModel(parent)
{

}

bool AllOpenFilesModel::setEditFlag(int iRow,bool bOn)
{
    if(iRow >= items.size() || iRow < 0)
        return false;

    items[iRow].bEdit = bOn;
    emit dataChanged(index(iRow), index(iRow), {Qt::DisplayRole});
    return true;
}

int AllOpenFilesModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return items.size();
}

QVariant AllOpenFilesModel::data(const QModelIndex &index, int role) const
{
    if(role == Qt::DisplayRole)
    {
        QString displayData = items[index.row()].fileInfo.fileName();
        if(items[index.row()].bEdit || items[index.row()].bTmp)
            displayData += "*";
        return displayData;
    }
    else if(role == Qt::ToolTipRole)
    {
        if(!items[index.row()].bTmp)
            return items[index.row()].fileInfo.absoluteFilePath();
        else
            return defTmpFileToolTip;
    }
    if(role == AllDataRole)
    {
        return QVariant::fromValue(items[index.row()]);
    }
    else
        return QVariant();
}

bool AllOpenFilesModel::setData(int iRow, const QVariant &value, int role)
{
    return setData(index(iRow), value, role);
}

//return row of file
//return -1 if there is no such file
int AllOpenFilesModel::findFile(QString fullPath)
{
    int res = -1;
    for(int i=0; i<items.size(); i++)
    {
        auto item = items[i];
        if(item.fileInfo.absoluteFilePath() == fullPath)
        {
            res = i;
            break;
        }
    }

    return res;
}

bool AllOpenFilesModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if(index.row() >= items.size() || index.row() < 0)
        return QAbstractListModel::setData(index,value,role);

    if(role == AllDataRole)
    {
        const auto & newData = qvariant_cast<EditFileData>(value);
        int iFindRow = findFile(newData.fileInfo.absoluteFilePath());
        if( iFindRow < 0 || iFindRow == index.row())
            items[index.row()] = newData;
    }
    else
        return QAbstractListModel::setData(index,value,role);

    emit dataChanged(index, index, {role});
    return true;
}

bool AllOpenFilesModel::insertRows(int row, int count, const QModelIndex &parent)
{
    if(row < 0 || (row > items.size() && items.size()!= 0))
        return false;
    beginInsertRows(parent,row, row+count-1);
    for(int i=row; i<row+count; i++)
    {
        items.insert(i, EditFileData());
    }
    endInsertRows();
    return true;
}

bool AllOpenFilesModel::removeRows(int row, int count, const QModelIndex &parent)
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
