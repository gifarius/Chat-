#include "connect.h"

QString Connector::connectToHost(QWidget &widget, const QString &ip, const QString &port) {
    socket = new QTcpSocket(&widget);
    socket->abort();
    socket->connectToHost(ip, port.toInt());

    if (!socket->waitForConnected(3000)) {
        return "Ошибка: " + socket->errorString();
    } else {
        return "Успешно подключено";
    }
}
