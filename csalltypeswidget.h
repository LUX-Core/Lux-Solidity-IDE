#ifndef CSALLTYPESWIDGET_H
#define CSALLTYPESWIDGET_H

#include <QWidget>

namespace Ui {
class CsAllTypesWidget;
}

class CsAllTypesWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CsAllTypesWidget(QWidget *parent = 0);
    ~CsAllTypesWidget();

private:
    Ui::CsAllTypesWidget *ui;
};

#endif // CSALLTYPESWIDGET_H
