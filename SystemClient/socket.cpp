#include "socket.h"

Socket::Socket() {

    socket = new QTcpSocket;

    // Connect socket signals to their respective slots
    connect(socket, &QAbstractSocket::connected, this, &Socket::Connected);
    connect(socket, &QAbstractSocket::disconnected, this, &Socket::disconnected);
    connect(socket, &QAbstractSocket::hostFound, this, &Socket::hostFound);
    connect(socket, &QAbstractSocket::errorOccurred, this, &Socket::errorOccurred);
    connect(socket, &QAbstractSocket::stateChanged, this, &Socket::stateChanged);
    connect(socket, &QAbstractSocket::readyRead, this, &Socket::readyRead);

}

Socket::~Socket()
{
    if (socket->isOpen()) {
        socket->close();
    }
    delete socket;
}

void Socket::connectToHost(QString hostname, quint16 port)
{
    if (socket->isOpen()) {
        disconnect();  // Ensure any existing connection is closed
    }

    qInfo() << "Trying to connect to " << hostname << " on port " << port;

    // Initiating the connection
    socket->connectToHost(hostname, port);

    // Adding a timeout for connection attempt (5 seconds)
    if (!socket->waitForConnected(5000)) {  // 5000ms = 5 seconds
        qInfo() << "Failed to connect within the timeout period.";
        return;
    }
}

void Socket::Connected()
{
    qInfo() << "Connected to Host";
}

void Socket::disconnected()
{
    qInfo() << "Connection Closed...";
}

void Socket::errorOccurred(QAbstractSocket::SocketError socketError)
{
    // Catch specific socket errors and print appropriate messages
    switch (socketError) {
    case QAbstractSocket::ConnectionRefusedError:
        qInfo() << "Error: Connection was refused by the server.";
        break;
    case QAbstractSocket::HostNotFoundError:
        qInfo() << "Error: The host was not found. Please check the hostname and port.";
        break;
    case QAbstractSocket::NetworkError:
        qInfo() << "Error: Network error occurred.";
        break;
    default:
        qInfo() << "Error: " << socketError << " " << socket->errorString();
        break;
    }
}

void Socket::hostFound()
{
    qInfo() << "Server Host is Found";
}

void Socket::stateChanged(QAbstractSocket::SocketState socketState)
{
    qInfo() << "State: " << socketState;
}

void Socket::disconnect()
{
    socket->close();
    socket->waitForDisconnected();  // Make it synchronous
}

void Socket::readyRead()
{
    qInfo() << "Reading Data from Server";

    QByteArray data = socket->readAll();  // Read all available data
    QTextStream in(&data);

    while (!in.atEnd()) {
        QString line = in.readLine();
        line.remove("\n");  // Remove any newline characters
        line.remove("\r");
        qDebug().noquote() << line;
    }
}

