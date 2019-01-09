#-------------------------------------------------
#
# Project created by QtCreator 2018-11-19T21:45:45
#
#-------------------------------------------------

QT  += core gui
QT  += network
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
    codeeditor.cpp \
    highlighter.cpp \
    find/findwidget.cpp \
    find/searchresulttreeitemdelegate.cpp \
    find/treewidgetfindresults.cpp \
    searchwgt.cpp \
    editcontract.cpp \
    allopenfilesmodel.cpp \
    scwizardwgt.cpp \
    scalltypeswidget.cpp \
    sctypesdelegate.cpp \
    sctypesmodel.cpp \
    sctypewidget.cpp \
    sctokenwgt.cpp

HEADERS += \
    codeeditor.h \
    global.h \
    highlighter.h \
    find/findwidget.h \
    find/globalsearch.h \
    find/searchresulttreeitemdelegate.h \
    find/treewidgetfindresults.h \
    searchwgt.h \
    editcontract.h \
    guiconstants.h \
    allopenfilesmodel.h \
    scwizardwgt.h \
    scalltypeswidget.h \
    sctypesdelegate.h \
    sctypesmodel.h \
    sctypewidget.h \
    sctokenwgt.h

FORMS += editcontract.ui \
    find/findwidget.ui \
    searchwgt.ui \
    scwizardwgt.ui \
    scalltypeswidget.ui \
    sctypewidget.ui \
    sctokenwgt.ui

RESOURCES += \
    res.qrc


unix:!macx{

LIBS += -L/media/alex/7a15ebc8-686e-4d37-aa51-198577f0f0dd/home/alex/Projects/openssl-1.0.2q/ -lssl
INCLUDEPATH += /media/alex/7a15ebc8-686e-4d37-aa51-198577f0f0dd/home/alex/Projects/openssl-1.0.2q/include
DEPENDPATH += /media/alex/7a15ebc8-686e-4d37-aa51-198577f0f0dd/home/alex/Projects/openssl-1.0.2q/include

LIBS += -L/media/alex/7a15ebc8-686e-4d37-aa51-198577f0f0dd/home/alex/Projects/openssl-1.0.2q/ -lcrypto
INCLUDEPATH += /media/alex/7a15ebc8-686e-4d37-aa51-198577f0f0dd/home/alex/Projects/openssl-1.0.2q/include
DEPENDPATH += /media/alex/7a15ebc8-686e-4d37-aa51-198577f0f0dd/home/alex/Projects/openssl-1.0.2q/include

install.path = $$OUT_PWD/SolCompilers
install.files += SolCompilers/*
INSTALLS +=install

contracts.path = $$OUT_PWD/SolCompilers/contracts
contracts.files += openzeppelin-solidity/contracts/*
INSTALLS +=contracts

LIBS += -L$$PWD/depends/lib/ -lquazip

INCLUDEPATH += $$PWD/depends/include
DEPENDPATH += $$PWD/depends/include
}

win32{

CONFIG(release, debug|release){
depends.path = $$OUT_PWD/release
depends.files = windows-dependecies/*
INSTALLS +=depends

install.path = $$OUT_PWD/release/SolCompilers
install.files += SolCompilers/*
INSTALLS +=install

contracts.path = $$OUT_PWD/release/SolCompilers/contracts
contracts.files += openzeppelin-solidity/contracts/*
INSTALLS +=contracts
}

CONFIG(debug, debug|release){
depends.path = $$OUT_PWD/debug
depends.files = windows-dependecies/*
INSTALLS +=depends

install.path = $$OUT_PWD/debug/SolCompilers
install.files += SolCompilers/*
INSTALLS +=install

contracts.path = $$OUT_PWD/debug/SolCompilers/contracts
contracts.files += openzeppelin-solidity/contracts/*
INSTALLS +=contracts
}

}

