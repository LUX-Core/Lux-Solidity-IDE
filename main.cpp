#include <QApplication>
#include <QFile>
#include "editcontract.h"

//#include <QJSEngine>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    {
        QFile file("://createContract.css");
        file.open(QFile::ReadOnly);
        a.setStyleSheet(QLatin1String(file.readAll()));
        file.close();
    }

//        QJSEngine myEngine;
//        QFile file("://soljson.js");
//        file.open(QFile::ReadOnly);
//        QByteArray ba = file.readAll();
//        file.close();
//        QJSValue fun = myEngine.evaluate(ba);
//        QFile file_sol("/media/alex/7a15ebc8-686e-4d37-aa51-198577f0f0dd/home/alex/Projects/sol_examples/example.sol");
//        file_sol.open(QFile::ReadOnly);
//        QString ba_sol =file_sol.readAll();
//        file_sol.close();
//        bool isCallable = fun.isCallable();
//        QJSValue res = fun.call(QJSValueList(std::initializer_list<QJSValue>
//        {QJSValue("compile"),QJSValue(ba_sol)}));
    EditContract w;
    w.show();
    return a.exec();
}
