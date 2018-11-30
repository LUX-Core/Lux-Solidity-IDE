#include "cstypewidget.h"
#include "ui_cstypewidget.h"
#include "cstypesmodel.h"

#include <QGraphicsDropShadowEffect>

csTypeWidget::csTypeWidget(const CsTypesItem & item, QWidget *parent) :
    QFrame(parent),
    ui(new Ui::csTypeWidget)
{
    ui->setupUi(this);
    ui->labelIcon->setPixmap(QPixmap(item.strIcon));
    ui->labelDescription->setText(item.strDescription);
    ui->labelType->setText(item.strType);
}

csTypeWidget::~csTypeWidget()
{
    delete ui;
}
