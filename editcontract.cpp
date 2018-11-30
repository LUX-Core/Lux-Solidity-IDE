#include <QShortcut>
#include <QFile>
#include <QTemporaryDir>
#include <QMessageBox>
#include <QDebug>
#include <QTemporaryDir>
#include <QDir>
#include <QMetaEnum>
#include <QRegExp>
#include <QTextCursor>
#include <QToolTip>

#include "editcontract.h"
#include "ui_editcontract.h"

EditContract::EditContract(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::EditContract)
{
    ui->setupUi(this);

    tmpDir = new QTemporaryDir();
    QDir(tmpDir->path()).mkpath("output");
    
    //toolsFrame
    {
        QShortcut * shortcut = new QShortcut(QKeySequence(tr("Ctrl+F")),
                                             this);
        connect(shortcut, &QShortcut::activated,
                this, &EditContract::slotSearchClicked);
        connect(ui->pushButtonSearch, &QPushButton::clicked,
                this, &EditContract::slotSearchClicked);
        connect(ui->pushButtonUndo, &QPushButton::clicked,
                ui->codeEdit, &CodeEditor::undo);
        connect(ui->pushButtonRedo, &QPushButton::clicked,
                ui->codeEdit, &CodeEditor::redo);

#ifdef __APPLE__
        ui->pushButtonSearch->setToolTip("Search (Cmd+F)");
        ui->pushButtonRedo->setToolTip("Redo (Cmd+Y)");
        ui->pushButtonUndo->setToolTip("Undo (Cmd+Z)");
#else
        ui->pushButtonSearch->setToolTip("Search (Ctrl+F)");
        ui->pushButtonRedo->setToolTip("Redo (Ctrl+Y)");
        ui->pushButtonUndo->setToolTip("Undo (Ctrl+Z)");
#endif
    }

    //buildFrame
    {
        QShortcut * shortcutBuild = new QShortcut(QKeySequence(tr("Ctrl+B")),
                                             this);
        connect(shortcutBuild, &QShortcut::activated,
                this, &EditContract::slotBuildClicked);
        connect(ui->pushButtonBuild, &QPushButton::clicked,
                this, &EditContract::slotBuildClicked);
#ifdef __APPLE__
        ui->pushButtonBuild->setToolTip("Build (Cmd+B)");
#else
        ui->pushButtonBuild->setToolTip("Build (Ctrl+B)");
#endif
        process_build = new QProcess(this);
        connect(process_build.data(), static_cast<void(QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished),
                this,
                [this](int exitCode, QProcess::ExitStatus exitStatus){ this->slotBuildFinished(exitCode, exitStatus);});
        connect(process_build.data(), &QProcess::errorOccurred,
                this, &EditContract::slotBuildProcError);
    }

    //find widget
    {
        ui->wgtFindResults->hide();
        connect(ui->wgtFindResults, &FindWidget::sigNeedHiding,
            this, [this](){ui->wgtFindResults->hide();});
        connect(ui->codeEdit, &CodeEditor::sigFindResults,
                ui->wgtFindResults, &FindWidget::slotReadyFindResults);
        connect(ui->wgtFindResults, &FindWidget::sigFindResultChoosed,
                ui->codeEdit, &CodeEditor::slotCurrentFindResultChanged);
    }
    fillInCompilerVersions();
    connect(ui->comboBoxCompilerVersion, &QComboBox::currentTextChanged,
            this, &EditContract::slotVersionChoosed);
    slotVersionChoosed(ui->comboBoxCompilerVersion->currentText());

    connect(ui->listWidgetErrWarnings, &QListWidget::itemClicked,
            this, &EditContract::slotErrWarningClicked);
    connect(ui->checkBoxOptimization, &QCheckBox::stateChanged,
            this, &EditContract::slotOptimizationStateChanged);
}

void EditContract::slotOptimizationStateChanged(int state)
{
    if(Qt::Unchecked == state)
        ui->widgetRuns->setEnabled(false);
    else
        ui->widgetRuns->setEnabled(true);
}


void EditContract::slotErrWarningClicked(QListWidgetItem *item)
{
    QString err_warning = item->text();
    QStringList params = err_warning.split(":",QString::SkipEmptyParts);
    if(params.size() > 3)
    {
        int nLine = params[1].toInt();
        int nOffset = params[2].toInt();
        auto block = ui->codeEdit->document()->findBlockByNumber(nLine-1);
        QTextCursor cursor = ui->codeEdit->textCursor();
        cursor.setPosition(block.position() + nOffset);
        ui->codeEdit->setTextCursor(cursor);
    }
}

void EditContract::slotVersionChoosed(QString newVersion)
{
    strCompileExe = QCoreApplication::applicationDirPath()
            + "/" + strCompilePath + "/" + newVersion + "/solc";
}

void EditContract::fillInCompilerVersions()
{
    QDir versionsDir(QCoreApplication::applicationDirPath() + "/" + strCompilePath);
    foreach(auto dir, versionsDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot))
    {
        ui->comboBoxCompilerVersion->addItem(dir);
    }
}

void EditContract::slotBuildFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    ui->pushButtonBuild->setEnabled(true);
    if(QProcess::CrashExit == exitStatus)
    {
        QMessageBox::critical(this, "Smart contract Build",
                              "Crash code - " + QString::number(exitCode));
        return;
    }
    //fill in comboBoxChooseDeploy
    {
        QDir deployDir(tmpDir->filePath("output"));
        foreach(auto fileInfo, deployDir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot))
        {
            if(ui->comboBoxChooseDeploy->findText(fileInfo.baseName()) < 0)
                ui->comboBoxChooseDeploy->addItem(fileInfo.baseName());
        }
    }
    //fill in err_warnings and labelBuildStatus
    {
        QString errorBuild = QString(process_build->readAllStandardError());

        QStringList err_warnings = errorBuild.split(tmpDir->filePath("tmp.sol"),
                                        QString::SkipEmptyParts);
        if(err_warnings.isEmpty())
            err_warnings = errorBuild.split(tmpDir->filePath("tmp.sol").replace("/", "\\"),
                                            QString::SkipEmptyParts);

        //fill in listWidgetErrWarnings
        foreach(auto strWarning, err_warnings)
        {
            ui->listWidgetErrWarnings->addItem("contract" + strWarning);
        }
        bool bSuccess = true;
        //add err_warnings to codeEdit
        QMap<int, ErrWarningBuildData> codeEditorWarnings;
        foreach(auto strWarning, err_warnings)
        {
            ErrWarningBuildData data;
            QStringList properties = strWarning.split(":", QString::SkipEmptyParts);
            if(properties.size() >= 4)
            {
                data.iY = properties[0].simplified().remove(" ").toInt();
                data.iX = properties[1].simplified().remove(" ").toInt();
                if(properties[2].contains("Error"))
                {
                    data.type = ErrWarningBuildData::iError;
                    bSuccess = false;
                }
                else
                {
                    data.type = ErrWarningBuildData::iWarning;
                }
                data.message = "contract" + strWarning;
            }
            codeEditorWarnings[data.iY-1] = data;
        }
        ui->codeEdit->setErr_Warnings(codeEditorWarnings);
        ui->codeEdit->update();
        if(bSuccess)
        {
            ui->labelBuildStatus->setText("Successful Build");
            ui->labelBuildStatus->setProperty("success", true);
            ui->pushButtonDeploy->setEnabled(true);
        }
        else
        {
            ui->labelBuildStatus->setText("Failed Build");
            ui->labelBuildStatus->setProperty("success", false);
            ui->pushButtonDeploy->setEnabled(false);
        }
        ui->labelBuildStatus->style()->unpolish(ui->labelBuildStatus);
        ui->labelBuildStatus->style()->polish(ui->labelBuildStatus);

        qDebug() << errorBuild;
    }

    QByteArray dataBuild = process_build->readAllStandardOutput();
    qDebug() << dataBuild;
}

void EditContract::slotBuildProcError(QProcess::ProcessError error)
{
    ui->pushButtonBuild->setEnabled(true);
    QMetaEnum metaEnum = QMetaEnum::fromType<QProcess::ProcessError>();
    QString strError = metaEnum.valueToKey(error);
    QMessageBox::critical(this, "Smart contract Build",
                          "Crash error - " + strError);
}

void EditContract::slotSearchClicked()
{
    if(search_Wgt.isNull())
    {
        search_Wgt = new SearchWgt(this);
        search_Wgt->show();
        //signals-slots connects
        {
            //mark
            connect(search_Wgt.data(), &SearchWgt::sigMark,
                    ui->codeEdit, &CodeEditor::slotSearchMark);

            //find
            connect(search_Wgt.data(), &SearchWgt::sigFindAllCurrentFile,
                    ui->codeEdit, &CodeEditor::slotFindAllCurrentFile);
            connect(search_Wgt.data(), &SearchWgt::sigFindAllCurrentFile,
                    this, [this](){ui->wgtFindResults->show();});
            connect(search_Wgt.data(), &SearchWgt::sigFindNext,
                    ui->codeEdit, &CodeEditor::slotFindNext);
            connect(search_Wgt.data(), &SearchWgt::sigFindPrev,
                    ui->codeEdit, &CodeEditor::slotFindPrev);

            //replace
            connect(search_Wgt.data(), &SearchWgt::sigReplace,
                    ui->codeEdit, &CodeEditor::slotReplace);
            connect(search_Wgt.data(), &SearchWgt::sigReplaceAllCurrent,
                    ui->codeEdit, &CodeEditor::slotReplaceAllCurrent);
        }
    }
    else
            search_Wgt->activateWindow();
}

void EditContract::slotBuildClicked()
{
    ui->pushButtonBuild->setEnabled(false);
    ui->comboBoxChooseDeploy->clear();
    ui->listWidgetErrWarnings->clear();
    QDir deployDir(tmpDir->filePath("output"));
    deployDir.removeRecursively();
    QDir(tmpDir->path()).mkpath("output");

    QString tmpPath = tmpDir->filePath("tmp.sol");
    QFile file_tmp(tmpPath);
    if(!file_tmp.open(QIODevice::WriteOnly))
    {
        QMessageBox::critical(this, "Smart contract Build",
                              "Can not create tmp file to build");
        return;
    }
    file_tmp.write(ui->codeEdit->document()->toPlainText().toLocal8Bit());
    file_tmp.close();
    QString params = " --bin --abi --overwrite ";
    if(ui->checkBoxOptimization->isChecked())
    {
        int nRuns = ui->spinBoxRuns->value();
        params = " --optimize-runs " + QString::number(nRuns) + params;
    }
    process_build->start(strCompileExe
                         + params + tmpPath
                         + " -o " + tmpDir->filePath("output"));
}

EditContract::~EditContract()
{
    tmpDir->remove();
    delete ui;
}
