#ifndef CREATECONTRACT_H
#define CREATECONTRACT_H

#include <QWidget>
#include <QPointer>
#include <QProcess>
#include <QMap>
#include <QStringList>
#include <QNetworkReply>
#include <QSettings>
#include "searchwgt.h"

class QTemporaryDir;
class QListWidgetItem;
class QNetworkAccessManager;

namespace Ui {
class EditContract;
}

class EditContract : public QWidget
{
    Q_OBJECT

public:
    explicit EditContract(QWidget *parent = 0);
    ~EditContract();
private:
    Ui::EditContract *ui;
    bool bUbuntu {false};
    QPointer<SearchWgt> search_Wgt;
    QPointer<QProcess> process_build;
    QPointer<QProcess> process_distrib;
    QTemporaryDir * tmpDir;
    QNetworkAccessManager * nam;
    QSettings settings;
    //test
    const QString strCompilePath{"SolCompilers"};
    QString strCompileExe {""};
    void fillInCompilerVersions();
    //get download links of solc
    //in https://github.com/ethereum/solidity/releases ubuntu and windows releases
    void getDownloadLinksSolc();
    QMap<QString, QString> downloadLinksSolc;
    void customizeComboBoxCompiler(int index, bool bDownload);
    void startBuild();
private slots:
    void slotDownSolcFinished();
    void slotProgressDownSolc(qint64 bytesReceived,
                              qint64 bytesTotal);
    void slotDownLinksSolcFinished();
    void slotProcDistribFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void slotSearchClicked();
    void slotBuildClicked();
    void slotBuildFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void slotBuildProcError(QProcess::ProcessError error);
    void slotVersionChoosed(QString newVersion);
    void slotErrWarningClicked(QListWidgetItem *item);
    void slotOptimizationStateChanged(int state);
    void slotChooseNewCompiler(int index);
};

#endif // CREATECONTRACT_H
