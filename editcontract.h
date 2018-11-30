#ifndef CREATECONTRACT_H
#define CREATECONTRACT_H

#include <QWidget>
#include <QPointer>
#include <QProcess>
#include <QStringList>
#include "searchwgt.h"

class QTemporaryDir;
class QListWidgetItem;

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
    QPointer<SearchWgt> search_Wgt;
    QPointer<QProcess> process_build;
    QTemporaryDir * tmpDir;
    //test
    const QString strCompilePath{"SolCompilers"};
    QString strCompileExe {""};
    void fillInCompilerVersions();

private slots:
    void slotSearchClicked();
    void slotBuildClicked();
    void slotBuildFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void slotBuildProcError(QProcess::ProcessError error);
    void slotVersionChoosed(QString newVersion);
    void slotErrWarningClicked(QListWidgetItem *item);
    void slotOptimizationStateChanged(int state);
};

#endif // CREATECONTRACT_H
