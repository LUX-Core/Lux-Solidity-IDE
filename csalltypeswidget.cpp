#include "csalltypeswidget.h"
#include "ui_csalltypeswidget.h"

#include "cstypesdelegate.h"
#include "cstypesmodel.h"

CsAllTypesWidget::CsAllTypesWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CsAllTypesWidget)
{
    ui->setupUi(this);

    //fill in cs types list
    {
        CsTypesModel * model = new CsTypesModel(this);
        model->insertRows(0,2);
        model->setData(0, "://imgs/contract.png", Qt::DecorationRole);
        model->setData(0, "Contract", CsTypesModel::TypeRole);
        model->setData(0, "DEscription of Contract", CsTypesModel::DescriptionRole);
        model->setData(1, "://imgs/cryptotoken.png", Qt::DecorationRole);
        model->setData(1, "Token", CsTypesModel::TypeRole);
        model->setData(1, "DEscription of Token", CsTypesModel::DescriptionRole);
        ui->listViewSC_Types->setModel(model);
        ui->listViewSC_Types->setItemDelegate(new CsTypesDelegate(this));
        ui->listViewSC_Types->setMouseTracking(true);
    }

}

CsAllTypesWidget::~CsAllTypesWidget()
{
    delete ui;
}
