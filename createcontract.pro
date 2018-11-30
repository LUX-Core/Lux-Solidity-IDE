#-------------------------------------------------
#
# Project created by QtCreator 2018-11-19T21:45:45
#
#-------------------------------------------------

QT       += core gui
#QT += qml
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = EditContract
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
    cstypewidget.cpp \
    cstypesmodel.cpp \
    cstypesdelegate.cpp \
    csalltypeswidget.cpp \
    codeeditor.cpp \
    highlighter.cpp \
    find/findwidget.cpp \
    find/searchresulttreeitemdelegate.cpp \
    find/treewidgetfindresults.cpp \
    searchwgt.cpp \
    editcontract.cpp

HEADERS += \
    cstypewidget.h \
    cstypesmodel.h \
    cstypesdelegate.h \
    csalltypeswidget.h \
    codeeditor.h \
    global.h \
    highlighter.h \
    find/findwidget.h \
    find/globalsearch.h \
    find/searchresulttreeitemdelegate.h \
    find/treewidgetfindresults.h \
    searchwgt.h \
    editcontract.h

FORMS += \
    cstypewidget.ui \
    EditContract.ui \
    csalltypeswidget.ui \
    find/findwidget.ui \
    searchwgt.ui \
    editcontract.ui

RESOURCES += \
    res.qrc

install.path = $$OUT_PWD/SolCompilers
install.files += SolCompilers/*
INSTALLS +=install
