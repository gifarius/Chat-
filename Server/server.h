#ifndef SERVER_H
#define SERVER_H

#include <QTcpServer>
#include <QTcpSocket>
#include <QList>
#include <QtGlobal>

class Server : public QTcpServer
{
    Q_OBJECT

public:
    explicit Server(QObject *parent = nullptr);

signals:
    void logMessage(const QString &message);

public slots:
    void startServer();
    void stopServer();

protected:
    void incomingConnection(qintptr socketDescriptor) override;

private slots:
    void onClientDisconnected();

private:
    QList<QTcpSocket*> clients;
};

#endif // SERVER_H
