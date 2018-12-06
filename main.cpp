#include <QApplication>
#include <QFile>
#include "editcontract.h"
#include "guiconstants.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);


    QApplication::setOrganizationName(QAPP_ORG_NAME);
    QApplication::setOrganizationDomain(QAPP_ORG_DOMAIN);
    QApplication::setApplicationName("Lux-Solidity-IDE");

    //setStyleSheet
    {
        QFile file("://createContract.css");
        file.open(QFile::ReadOnly);
        a.setStyleSheet(QLatin1String(file.readAll()));
        file.close();
    }

    EditContract w;
    w.show();
    return a.exec();
}
