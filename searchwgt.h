#ifndef SEARCHWGT_H
#define SEARCHWGT_H

#include <QWidget>
#include <QSettings>

namespace Ui {
class SearchWgt;
}

class SearchWgt : public QWidget
{
    Q_OBJECT

public:
    explicit SearchWgt(QWidget *parent = 0);
    ~SearchWgt();
protected:
    bool event(QEvent *e);
private:
    Ui::SearchWgt *ui;
    QSettings settings;
private slots:
    //transparency
    void slotSliderTransparencyChanged(int value);
    void slotTransparencyGroupClicked(bool checked);
    void slotOnLosingFocusClicked();
    void slotAllwaysClicked();

    //options
    void slotRegularClicked(bool checked);
    void slotCaseClicked(bool checked);
    void slotWholeWordClicked(bool checked);
    void slotWrapAroundClicked(bool checked);

    //choose find-replace
    void slotReplaceRadButClicked();
    void slotFindRadButClicked();

    //Find
    void slotFindNext();
    void slotFindPrev();
    void slotFindAllCurrent();

    //clear
    void slotClearMarks();

    //replace
    void slotReplace();
    void slotReplaceAllCurrent();
signals:

    //mark
    void sigMark(QString strSearch);

    //find
    void sigFindAllCurrentFile();
    void sigFindAllAllFiles();
    void sigFindNext();
    void sigFindPrev();

    //replace
    void sigReplace(QString strReplace);
    void sigReplaceAllCurrent(QString strReplace);
    void sigReplaceAllAll(QString strReplace);
};

#endif // SEARCHWGT_H
