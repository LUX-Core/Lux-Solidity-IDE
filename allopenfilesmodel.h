#ifndef ALLOPENFILESMODEL_H
#define ALLOPENFILESMODEL_H
#include <QAbstractListModel>
#include <QFileInfo>

struct EditFileData
{
    EditFileData()
    {}
    EditFileData(QFileInfo fileInfo, bool bTmp):
        fileInfo(fileInfo),
        bTmp(bTmp),
        bEdit(false)
    {}
    QFileInfo fileInfo;
    bool bTmp {false};
    bool bEdit{false};
};
Q_DECLARE_METATYPE(EditFileData)

class AllOpenFilesModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum DataRole{
        AllDataRole = Qt::UserRole + 1
    };

    AllOpenFilesModel(QObject *parent = nullptr);
    //return row of file
    //return -1 if there is no such file
    int findFile(QString fullPath);
    bool setEditFlag(int iRow, bool bOn);

    //override functions
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;

    bool setData(int iRow, const QVariant &value, int role = Qt::EditRole);
private:
    QList<EditFileData> items;
};

#endif // ALLOPENFILESMODEL_H
