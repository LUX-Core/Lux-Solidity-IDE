#ifndef CSALLTYPESWIDGET_H
#define CSALLTYPESWIDGET_H

#include <QWidget>

namespace Ui {
class CsAllTypesWidget;
}

class ScAllTypesWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ScAllTypesWidget(QWidget *parent = 0);
    ~ScAllTypesWidget();

private:
    enum eTypeContract {_TokenType,
                       _CrowdsaleType,
                       _numTypes};
    Ui::CsAllTypesWidget *ui;
};

#endif // CSALLTYPESWIDGET_H
