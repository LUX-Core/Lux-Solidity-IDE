#include "scwizardwgt.h"
#include "ui_scwizardwgt.h"

ScWizardWgt::ScWizardWgt(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::scWizardWgt)
{
    ui->setupUi(this);
}

ScWizardWgt::~ScWizardWgt()
{
    delete ui;
}
