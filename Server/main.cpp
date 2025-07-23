#include <QApplication>
#include <QNetworkProxy>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QNetworkProxyFactory::setUseSystemConfiguration(false);

    MainWindow window;
    window.show();

    return app.exec();
}
