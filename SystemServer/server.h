#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QDebug>
#include <QTcpServer>
#include <QTcpSocket>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QFile>
#include <QMap>
#include <QHostAddress>
#include <QThread>

#include"DataBase.h"

class MyServer : public QTcpServer
{
    Q_OBJECT
public:
    explicit MyServer(QObject *parent = nullptr);

signals:

private slots:
    void ReadRequest();
public:
    /*Log in Function*/
    void processPostRequestlogin(QTcpSocket* socket, const QByteArray& requestData);
    bool searchInDatabase(const QString& name, QString& Password, QString& authority);

    /*Get counterID */
    void processgetRequestgetcounterID(QTcpSocket* socket, const QByteArray& requestData);

    /*Pay Bill*/
    void processPostRequestpaybill(QTcpSocket* socket, const QByteArray& requestData);

    /*Add Bill*/
    void processPostRequestaddbill(QTcpSocket* socket, const QByteArray& requestData, QJsonObject &database);

    /*Get Bills DataBase*/
    void processgetRequestlistbills(QTcpSocket* socket);

    /*Get Users DataBase*/
    void processgetRequestlistusers(QTcpSocket* socket);

    /*Create User*/
    void processPostRequestcreateuser(QTcpSocket* socket, const QByteArray& requestData,QJsonObject& database);

    /*Update User*/
    void processPostRequestupdateuser(QTcpSocket* socket, const QByteArray& requestData,QJsonObject& database);

    /*send Response*/
    void sendResponse(QTcpSocket* socket, const QString& responseMessage);

    /*Delete User*/
    void processDeleteRequestdeleteuser(QTcpSocket* socket, const QByteArray& requestData,QJsonObject& database);

private:
    QTcpServer server;

protected:
    void incomingConnection(qintptr socketDescriptor) override;
};

#endif // SERVER_H
