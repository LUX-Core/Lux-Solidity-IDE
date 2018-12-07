﻿#include <QShortcut>
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
#include <QProcess>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QPalette>
#include <QFileDialog>
#include <QLabel>
#include <QFontMetrics>

#include "editcontract.h"
#include "ui_editcontract.h"

//settings
#define defSolcVersion "SolcVersion"

//file with local solc compilers
#define defLocalSolcFile "local"
#define defPathCompiler "path"
#define defVersionCompiler "version"

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

    nam = new QNetworkAccessManager(this);
    #ifdef _WIN32
        getDownloadLinksSolc();
    #endif
    //detect bUbuntu
    {
    #ifdef __linux__
        process_distrib = new QProcess(this);
        connect(process_distrib.data(), static_cast<void(QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished),
                this, [this](int exitCode, QProcess::ExitStatus exitStatus){ this->slotProcDistribFinished(exitCode, exitStatus);});
        process_distrib->start("lsb_release", QStringList() << "-i");
    #endif
    }

    //ui->comboBoxCompilerVersion
    {
        fillInCompilerVersions();
        connect(ui->comboBoxCompilerVersion, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
              [=](int index){ slotChooseNewCompiler(index); });
        slotChooseNewCompiler(ui->comboBoxCompilerVersion->currentIndex());
    }

    connect(ui->listWidgetErrWarnings, &QListWidget::itemClicked,
            this, &EditContract::slotErrWarningClicked);
    connect(ui->checkBoxOptimization, &QCheckBox::stateChanged,
            this, &EditContract::slotOptimizationStateChanged);

    ui->progressBarDownloadSolc->hide();
    ui->listWidgetErrWarnings->installEventFilter(this);

    connect(ui->pushButtonAddCompiler, &QPushButton::clicked,
            this, &EditContract::slotAddSolcManually);
}

//get download links of solc
//in https://github.com/ethereum/solidity/releases ubuntu and windows releases
void EditContract::getDownloadLinksSolc()
{
    QNetworkRequest req (QUrl("https://api.github.com/repos/ethereum/solidity/releases"));
    auto reply = nam->get(req);
    connect(reply, &QNetworkReply::finished,
            this, &EditContract::slotDownLinksSolcFinished);
}

void EditContract::slotAddSolcManually()
{
    QString newSolc = QFileDialog::getOpenFileName(this, tr("Add Solc Compiler"),QCoreApplication::applicationDirPath(),
                                 "solc");
    if(! newSolc.isEmpty())
    {
        QString version;
        //fill in version
        {
            QProcess proc;
            proc.start(newSolc, QStringList() << "--version");
            proc.waitForFinished(1000);
            QRegExp reg_exp("\\d{1,2}[.]\\d{1,2}[.]\\d{1,2}");     
            QString output = proc.readAllStandardOutput();
            int pos = reg_exp.indexIn(output);
            if (pos != -1) 
                version = output.mid(pos);
        }
        if(version.isEmpty())
        {
            QMessageBox::critical(this, tr("Add Solc Compiler"), tr("It is not solc compiler!"));
            return;
        }
        
        QString localSolcFileName = QCoreApplication::applicationDirPath() + "/" + strCompilePath + "/" + QString(defLocalSolcFile);
        QFile file(localSolcFileName);
        QJsonDocument documentOld;
        if(file.open(QIODevice::ReadOnly))
        {
            documentOld = QJsonDocument::fromJson(file.readAll());
            file.close();
        }
        QJsonArray array = documentOld.array();
        QJsonObject newObj;
        newObj[defPathCompiler] = newSolc;
        newObj[defVersionCompiler] = version; 
        array.append(newObj);
        QJsonDocument documentNew(array);            
        if(file.open(QIODevice::Append))
        {
            file.write(documentNew.toJson());
            file.close();
        }
        pathsSolc[version] = newSolc;
        ui->comboBoxCompilerVersion->addItem(version.remove("\n").remove("\r"));
        customizeComboBoxCompiler(ui->comboBoxCompilerVersion->count() - 1, false);
        ui->comboBoxCompilerVersion->setCurrentText(version);
    }
}

void EditContract::slotDownLinksSolcFinished()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply *>(sender());
    auto data = reply->readAll();
    QJsonDocument document = QJsonDocument::fromJson(data);
    auto array = document.array();
    for(int i=0; i<array.size(); i++)
    {
        QJsonObject object = array[i].toObject();
        QString nameVersion = object["tag_name"].toString().remove("v");
        if(nameVersion.size() > 2)
        {
            if(nameVersion.at(nameVersion.size()-2) == '.')
            {
                nameVersion.insert(nameVersion.size()-1, "0");
            }
        }
        QJsonArray assetsArray = object["assets"].toArray();
        for(int iAsset=0; iAsset<assetsArray.size(); iAsset++)
        {
            QJsonObject asset = assetsArray[iAsset].toObject();
            QString namePackage = asset["name"].toString();
            if(bUbuntu)
            {
                if(namePackage.contains("ubuntu"))
                {
                    QString downloadUrl = asset["browser_download_url"].toString();
                    downloadLinksSolc[nameVersion] = downloadUrl;
                    break;
                }
            }
        }
    }

    //fill in comboBoxCompilerVersion
    foreach(QString name, downloadLinksSolc.keys())
    {
        if(ui->comboBoxCompilerVersion->findText(name) < 0)
        {
            ui->comboBoxCompilerVersion->addItem(name);
            customizeComboBoxCompiler(ui->comboBoxCompilerVersion->count() - 1, true);
        }

    }
}

void EditContract::customizeComboBoxCompiler(int index, bool bDownload)
{
    if(bDownload)
    {
        ui->comboBoxCompilerVersion->setItemIcon(index, QIcon("://imgs/download.png"));
        ui->comboBoxCompilerVersion->setItemData(index,
                                                 QColor(Qt::gray), Qt::ForegroundRole);
    }
    else
    {
        ui->comboBoxCompilerVersion->setItemIcon(index, QIcon("://imgs/Check.png"));
        ui->comboBoxCompilerVersion->setItemData(index,
                                                 QColor(Qt::black), Qt::ForegroundRole);
    }

    ui->comboBoxCompilerVersion->setItemData(index,bDownload);

    if(index==ui->comboBoxCompilerVersion->currentIndex())
        slotChooseNewCompiler(index);
    ui->comboBoxCompilerVersion->model()->sort(0);
}


void EditContract::slotChooseNewCompiler(int index)
{

    bool bDownload = ui->comboBoxCompilerVersion->itemData(index).toBool();

    if(bDownload)
    {
        QPalette pal = ui->comboBoxCompilerVersion->palette();
        pal.setBrush(QPalette::ButtonText, Qt::gray);
        ui->comboBoxCompilerVersion->setPalette(pal);
    }
    else
    {
        QPalette pal = ui->comboBoxCompilerVersion->palette();
        pal.setBrush(QPalette::ButtonText, Qt::black);
        ui->comboBoxCompilerVersion->setPalette(pal);
    }
}


void EditContract::slotProcDistribFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    Q_UNUSED(exitCode)
    if(QProcess::NormalExit == exitStatus)
    {
        auto baData = process_distrib->readAllStandardOutput();
        if(baData.contains("Ubuntu"))
        {
            bUbuntu = true;
            getDownloadLinksSolc();
        }
    }
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

void EditContract::fillInCompilerVersions()
{
    QDir versionsDir(QCoreApplication::applicationDirPath() + "/" + strCompilePath);
    foreach(auto dir, versionsDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot))
    {
        QDir subDir(versionsDir.absoluteFilePath(dir));
        if(subDir.entryList(QDir::Files).contains("solc"))
        {
            ui->comboBoxCompilerVersion->addItem(dir);
            //TODO add windows,...

#ifdef __linux__
            pathsSolc[dir] = QCoreApplication::applicationDirPath() + "/" + strCompilePath
                    + "/" + dir + "/solc";
#endif
            customizeComboBoxCompiler(ui->comboBoxCompilerVersion->count() - 1, false);
        }
    }
    
    QString localSolcFileName = QCoreApplication::applicationDirPath() + "/" + strCompilePath + "/" + QString(defLocalSolcFile);
    QFile file(localSolcFileName);
    QJsonDocument documentOld;
    if(file.open(QIODevice::ReadOnly))
    {
        documentOld = QJsonDocument::fromJson(file.readAll());
        file.close();
    }
    QJsonArray array = documentOld.array();
    for(int i=0; i<array.size(); i++)
    {
        auto obj = array[i].toObject();
        QString path = obj[defPathCompiler].toString();
        QString version = obj[defVersionCompiler].toString();
        if(!path.isEmpty()
                &&
           !version.isEmpty())
        {
            ui->comboBoxCompilerVersion->addItem(version);
            pathsSolc[version] = path;
            customizeComboBoxCompiler(ui->comboBoxCompilerVersion->count() - 1, false);
        }
    }
    
    QString savedVersion = settings.value(defSolcVersion).toString();
    if(ui->comboBoxCompilerVersion->findText(savedVersion))
        ui->comboBoxCompilerVersion->setCurrentText(savedVersion);
}

bool EditContract::eventFilter(QObject *watched, QEvent *event)
{
    if(ui->listWidgetErrWarnings == qobject_cast<QListWidget*>(watched))
    {
        if(QEvent::Resize == event->type())
        {
            for(int i=0; i<ui->listWidgetErrWarnings->count(); i++)
            {
                auto index = ui->listWidgetErrWarnings->model()->index(i,0);
                auto item = ui->listWidgetErrWarnings->item(i);
                QLabel * label = qobject_cast<QLabel *>(ui->listWidgetErrWarnings->indexWidget(index));
                QFontMetrics fm(item->font());
                //TODO understand why 4*spacing (not 2)...
                int height = fm.boundingRect(QRect(0,0, ui->listWidgetErrWarnings->width() - 4*ui->listWidgetErrWarnings->spacing(), 100),
                                           Qt::TextWordWrap, label->text()).size().height();
                label->setMinimumSize(QSize(ui->listWidgetErrWarnings->width() - 4*ui->listWidgetErrWarnings->spacing(),
                                            height));
                item->setSizeHint(QSize(ui->listWidgetErrWarnings->width() - 4*ui->listWidgetErrWarnings->spacing(),
                                        height));
            }
        }
    }

    QListWidgetItem * item = reinterpret_cast<QListWidgetItem *>(watched->property("item").toLongLong());
    if(nullptr != item)
    {
        if(QEvent::MouseButtonPress == event->type())
        {
            item->setSelected(true);
            return true;
        }
    }

    return QWidget::eventFilter(watched, event);
}

void EditContract::slotBuildFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    ui->pushButtonBuild->setEnabled(true);
    ui->pushButtonAddCompiler->setEnabled(true);
    ui->comboBoxCompilerVersion->setEnabled(true);
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
            QString textItem;
            if(strWarning.contains(QCoreApplication::applicationDirPath() + "/" + strCompilePath))
                textItem = strWarning;
            else
                textItem = "contract" + strWarning;
            QListWidgetItem * item = new QListWidgetItem();
            ui->listWidgetErrWarnings->addItem(item);
            QLabel * label = new QLabel(ui->listWidgetErrWarnings);
            label->setWordWrap(true);
            label->setTextInteractionFlags(Qt::TextSelectableByMouse);
            label->setText(textItem);
            label->setIndent(10);
            label->setAlignment(Qt::AlignLeft);
            label->setProperty("item", reinterpret_cast<qlonglong>(item));
            label->installEventFilter(this);
            //TODO understand why 4*spacing (not 2)...
            QFontMetrics fm(item->font());
            int height = fm.boundingRect(QRect(0,0, ui->listWidgetErrWarnings->width() - 4*ui->listWidgetErrWarnings->spacing(), 100),
                                       Qt::TextWordWrap, textItem).size().height();
            label->setMinimumSize(QSize(ui->listWidgetErrWarnings->width() - 4*ui->listWidgetErrWarnings->spacing(),
                                        height));
            item->setSizeHint(QSize(ui->listWidgetErrWarnings->width() - 4*ui->listWidgetErrWarnings->spacing(),
                                    height));
            ui->listWidgetErrWarnings->setItemWidget(item,
                                                      label);

        }
        bool bSuccess = true;
        //add err_warnings to codeEdit
        QMap<int, ErrWarningBuildData> codeEditorWarnings;
        foreach(auto strWarning, err_warnings)
        {
            if(strWarning.contains(QCoreApplication::applicationDirPath() + "/" + strCompilePath)
                    &&
               strWarning.toLower().contains("error"))
            {
                bSuccess = false;
                break;
            }
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

void EditContract::slotProgressDownSolc(qint64 bytesReceived, qint64 bytesTotal)
{
    if(bytesTotal != 0)
        ui->progressBarDownloadSolc->setValue((bytesReceived*100)/bytesTotal);
}

void EditContract::slotBuildProcError(QProcess::ProcessError error)
{
    ui->pushButtonBuild->setEnabled(true);
    ui->pushButtonAddCompiler->setEnabled(true);
    ui->comboBoxCompilerVersion->setEnabled(true);
    QMetaEnum metaEnum = QMetaEnum::fromType<QProcess::ProcessError>();
    QString strError = metaEnum.valueToKey(error);
    QMessageBox::critical(this, tr("Smart contract Build"),
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
    bool bDownload = ui->comboBoxCompilerVersion->itemData(
                ui->comboBoxCompilerVersion->currentIndex()).toBool();
    if(!bDownload)
        startBuild();
    else
    {
        ui->comboBoxCompilerVersion->setEnabled(false);
        ui->pushButtonBuild->setEnabled(false);
        ui->pushButtonAddCompiler->setEnabled(false);
        QString downloadLink = downloadLinksSolc[ui->comboBoxCompilerVersion->currentText()];
        auto reply = nam->get(QNetworkRequest(QUrl(downloadLink)));
        reply->setProperty("Version", ui->comboBoxCompilerVersion->currentText());
        connect(reply, &QNetworkReply::finished,
                this, &EditContract::slotDownSolcFinished);
    }
}

void EditContract::slotDownSolcFinished()
{
    auto reply = qobject_cast<QNetworkReply *>(sender());

    if(reply->error() != QNetworkReply::NoError)
    {
        ui->comboBoxCompilerVersion->setEnabled(true);
        ui->pushButtonBuild->setEnabled(true);
        ui->progressBarDownloadSolc->setEnabled(false);
        QString data = QMetaEnum::fromType<QNetworkReply::NetworkError>().valueToKey(reply->error());
        ui->labelBuildStatus->setText(tr("Could not download compiler. Error - ") + data);
        ui->labelBuildStatus->setProperty("success", false);
        ui->labelBuildStatus->style()->unpolish(ui->labelBuildStatus);
        ui->labelBuildStatus->style()->polish(ui->labelBuildStatus);
        return;
    }

    //redirect
    {
        QVariant redirectUrl =
                 reply->attribute(QNetworkRequest::RedirectionTargetAttribute);

        if(!redirectUrl.toUrl().isEmpty())
        {
            ui->progressBarDownloadSolc->show();
            ui->progressBarDownloadSolc->setValue(0);
            auto replyNew = nam->get(QNetworkRequest(redirectUrl.toUrl()));
            replyNew->setProperty("Version", reply->property("Version"));
            connect(replyNew, &QNetworkReply::finished,
                    this, &EditContract::slotDownSolcFinished);
            connect(replyNew, &QNetworkReply::downloadProgress,
                    this, &EditContract::slotProgressDownSolc);
            return;
        }
    }

    ui->progressBarDownloadSolc->hide();

    QByteArray dataReply = reply->readAll();
    QDir versionsDir(QCoreApplication::applicationDirPath() + "/" + strCompilePath);
    QString version = reply->property("Version").toString();
    versionsDir.mkdir(version);
    bool bSuccess = false;
    //TODO add windows,...
    if(bUbuntu)
    {
        QFile file_compiler(versionsDir.absoluteFilePath(version + "/solc.zip"));
        if(file_compiler.open(QIODevice::WriteOnly))
        {
            file_compiler.write(dataReply);
            file_compiler.close();
            QProcess proc;
            proc.setWorkingDirectory(versionsDir.absoluteFilePath(version));
            proc.start("unzip", QStringList() << "solc.zip" << "-d" << "output");
            proc.waitForFinished();
            if(QProcess::NormalExit == proc.exitStatus())
            {
                QFile::remove(versionsDir.absoluteFilePath(version + "/solc.zip"));
                QDir output(versionsDir.absoluteFilePath(version + "/output"));
                auto files = output.entryList(QDir::Files);
                bSuccess = true;
                foreach(auto file, files)
                {
                    if(file.contains("sol"))
                        bSuccess &= QFile::copy(versionsDir.absoluteFilePath(version + "/output/" + file),
                                    versionsDir.absoluteFilePath(version + "/" + file));
                }
                output.removeRecursively();
            }
        }
    }

    if(bSuccess)
    {
        pathsSolc[version]
                = versionsDir.absoluteFilePath(version + "/solc");
        customizeComboBoxCompiler(ui->comboBoxCompilerVersion->currentIndex(), false);
        startBuild();
    }
    else
    {
        ui->labelBuildStatus->setText(tr("Could not create new compiler"));
        ui->labelBuildStatus->setProperty("success", false);
        ui->labelBuildStatus->style()->unpolish(ui->labelBuildStatus);
        ui->labelBuildStatus->style()->polish(ui->labelBuildStatus);
    }
}


void EditContract::startBuild()
{
    settings.setValue(defSolcVersion, ui->comboBoxCompilerVersion->currentText());
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
        QMessageBox::critical(this, tr("Smart contract Build"),
                              tr("Can not create tmp file to build"));
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
    QString version = ui->comboBoxCompilerVersion->currentText();
    process_build->start(pathsSolc[version]
                         + params + tmpPath
                         + " -o " + tmpDir->filePath("output"));
}

EditContract::~EditContract()
{
    tmpDir->remove();
    delete ui;
}
