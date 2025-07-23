#ifndef CONNECT_H
#define CONNECT_H

#include <QString>
#include <QTcpSocket>
#include <QWidget>

class Connector {
public:
    QString connectToHost(QWidget &widget, const QString &ip, const QString &port);
    QTcpSocket* getSocket() const { return socket; }

private:
    QTcpSocket* socket = nullptr;
};

#endif // CONNECT_H
