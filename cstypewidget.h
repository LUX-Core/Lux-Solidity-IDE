#ifndef CSTYPEWIDGET_H
#define CSTYPEWIDGET_H

#include <QFrame>
#include "cstypesmodel.h"

namespace Ui {
class csTypeWidget;
}

class csTypeWidget : public QFrame
{
    Q_OBJECT

public:
    explicit csTypeWidget(const CsTypesItem & item, QWidget *parent = 0);
    ~csTypeWidget();
private:
    Ui::csTypeWidget *ui;
};

#endif // CSTYPEWIDGET_H
