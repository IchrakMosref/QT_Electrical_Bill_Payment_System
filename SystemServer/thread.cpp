#include "thread.h"

Thread::Thread(qintptr socketDescriptor, QObject *parent)
    : QThread{parent},socketDescriptor(socketDescriptor)
{
}

void Thread::run()
{
    //Thread starts here
    qInfo() << socketDescriptor << " Starting Thread";
    socket = new QTcpSocket();
    if(!socket->setSocketDescriptor(this->socketDescriptor))
        {
            emit error(socket->error());
            return;
        }
    connect(socket,SIGNAL(&QAbstractSocket::readyRead()),this,SLOT(&Thread::readyRead()),Qt::DirectConnection);
    connect(socket,SIGNAL(&QAbstractSocket::disconnected()),this,SLOT(&Thread::disconnected()),Qt::DirectConnection);

    qInfo() << socketDescriptor << " Client connected";

    exec();

}

void Thread::disconnected()
{
    qInfo() << socketDescriptor << " disconnected";

    socket->deleteLater();
    exit(0);
}

void Thread::readyRead()
{
    QByteArray Data = socket->readAll();
    qInfo() << socketDescriptor << " Data in : " << Data;
    socket->write(Data);
}
