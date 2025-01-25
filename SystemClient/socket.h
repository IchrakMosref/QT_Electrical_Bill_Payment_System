#ifndef SOCKET_H
#define SOCKET_H

#include <QObject>
#include <QTcpSocket>
#include <QDebug>
#include <QAbstractSocket>

class Socket : public QObject
{
    Q_OBJECT
public:

    Socket();
    ~Socket();

signals:

public slots:
    void connectToHost(QString hostname, quint16 port);
    void errorOccurred(QAbstractSocket::SocketError socketError);
    void hostFound();
    void stateChanged(QAbstractSocket::SocketState socketState);
    void disconnect();
    void readyRead();
    void Connected();
    void disconnected();

public:
    QTcpSocket *socket;

};

#endif // SOCKET_H
