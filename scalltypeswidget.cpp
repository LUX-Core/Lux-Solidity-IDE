#include "scalltypeswidget.h"
#include "ui_csalltypeswidget.h"

#include "sctypesdelegate.h"
#include "sctypesmodel.h"

ScAllTypesWidget::ScAllTypesWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CsAllTypesWidget)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);
#if defined(WIN32) || defined(WIN64) || defined(__linux__)
    setWindowFlags(Qt::Window);
    setWindowFlag(Qt::WindowMinimizeButtonHint, false);
    setWindowFlag(Qt::WindowMaximizeButtonHint, false);
#endif
#ifdef __APPLE__
    setWindowFlags(Qt::Window|Qt::WindowStaysOnTopHint
                   | Qt::WindowTitleHint | Qt::WindowCloseButtonHint | Qt::CustomizeWindowHint);
#endif
    //fill in cs types list
    {
        ScTypesModel * model = new ScTypesModel(this);
        model->insertRows(0,_numTypes);
        model->setData(_CrowdsaleType, "", Qt::DecorationRole);
        model->setData(_CrowdsaleType, "Crowdsale contract", ScTypesModel::TypeRole);
        model->setData(_CrowdsaleType, "Start your ICO/Token sale", ScTypesModel::DescriptionRole);
        model->setData(_TokenType, "://imgs/cryptotoken.png", Qt::DecorationRole);
        model->setData(_TokenType, "Token contract", ScTypesModel::TypeRole);
        model->setData(_TokenType, "Create a Token", ScTypesModel::DescriptionRole);
        ui->listViewSC_Types->setModel(model);
        ui->listViewSC_Types->setItemDelegate(new ScTypesDelegate(this));
        ui->listViewSC_Types->setMouseTracking(true);
    }

}

ScAllTypesWidget::~ScAllTypesWidget()
{
    delete ui;
}
