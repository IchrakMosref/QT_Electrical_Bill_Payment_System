#ifndef THREAD_H
#define THREAD_H

#include <QObject>
#include <QTcpSocket>
#include <QThread>


class Thread : public QThread
{
    Q_OBJECT
public:
    explicit Thread(qintptr socketDescriptor ,QObject *parent = nullptr);
    void run();

signals:
    void error(QTcpSocket::SocketError socketerror);

public slots:
    void readyRead();
    void disconnected();

private:
    QTcpSocket *socket;
    qintptr socketDescriptor;
};

#endif // THREAD_H
