#include "server.h"
#include <QDebug>

Server::Server(QObject *parent) : QTcpServer(parent) {}

void Server::startServer()
{
    if (!listen(QHostAddress::Any, 1234)) {
        emit logMessage(QString("Server failed to start: %1").arg(errorString()));
    } else {
        emit logMessage(QString("Server started on port %1").arg(serverPort()));
    }
}

void Server::stopServer()
{
    for (QTcpSocket* client : clients) {
        client->disconnectFromHost();
        client->close();
        client->deleteLater();
    }
    clients.clear();
    close();
    emit logMessage("Server stopped.");
}

void Server::incomingConnection(qintptr socketDescriptor)
{
    QTcpSocket* clientSocket = new QTcpSocket(this);
    clientSocket->setSocketDescriptor(socketDescriptor);
    clients.append(clientSocket);

    connect(clientSocket, &QTcpSocket::disconnected, this, &Server::onClientDisconnected);

    connect(clientSocket, &QTcpSocket::readyRead, [clientSocket, this]() {
        QByteArray data = clientSocket->readAll();
        QString message = QString("Received from client: %1").arg(QString(data));
        emit logMessage(message);
        clientSocket->write("Acknowledged: " + data);
    });

    emit logMessage(QString("New client connected: %1").arg(socketDescriptor));
}

void Server::onClientDisconnected()
{
    QTcpSocket* client = qobject_cast<QTcpSocket*>(sender());
    if (client) {
        clients.removeAll(client);
        client->deleteLater();
        emit logMessage("Client disconnected.");
    }
}
