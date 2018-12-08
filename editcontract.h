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
protected:
    bool eventFilter(QObject *watched, QEvent *event) override;
private:
    Ui::EditContract *ui;
#ifdef __linux__
    bool bUbuntu {false};
#endif
    QPointer<SearchWgt> search_Wgt;
    QPointer<QProcess> process_build;
    QPointer<QProcess> process_linux_distrib;
    QTemporaryDir * tmpDir;
    QNetworkAccessManager * nam;
    QSettings settings;
    //test
    const QString strCompilePath{"SolCompilers"};
    void fillInCompilerVersions();
    //get download links of solc
    //in https://github.com/ethereum/solidity/releases ubuntu and windows releases
    void getDownloadLinksSolc();
    QMap<QString, QString> downloadLinksSolc; //<version, downloadLink>
    QMap<QString, QString> pathsSolc; //<version, path>
    void customizeComboBoxCompiler(int index, bool bDownload);
    void startBuild();
private slots:
    void slotAddSolcManually();
    void slotDownSolcFinished();
    void slotProgressDownSolc(qint64 bytesReceived,
                              qint64 bytesTotal);
    void slotDownLinksSolcFinished();
#ifdef __linux__
    void slotProcDistribFinished(int exitCode, QProcess::ExitStatus exitStatus);
#endif
    void slotSearchClicked();
    void slotBuildClicked();
    void slotBuildFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void slotBuildProcError(QProcess::ProcessError error);
    void slotErrWarningClicked(QListWidgetItem *item);
    void slotOptimizationStateChanged(int state);
    void slotChooseNewCompiler(int index);
};

#endif // CREATECONTRACT_H
