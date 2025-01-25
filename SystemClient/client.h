#ifndef CLIENT_H
#define CLIENT_H

#include "socket.h"
#include "httprequestsender.h"

#include <QObject>
#include <QString>
#include <QJsonObject>
#include <QJsonDocument>
#include <QDebug>

class Client : public QObject
{
    Q_OBJECT

public:
    Client();
    ~Client();
    void setHttpRequestSender(HttpRequestSender* sender);

public slots:
    void Log_in(QString username ,QString pass,QString authority); //user+admin
    void Get_CounterID(QString CIN); //user + admin
    void Pay_Bill(QString counterID); //user
    void listBills(); //admin
    void listUsers(); //admin
    void Create_User(QString User_Data); // admin
    void Update_User(QString counterID,QString User_Data); // admin
    void Delete_User(QString CIN); // admin
    void Add_Bill(QString counterID,QString Bill_Data); // admin


private:
    Socket socket;
    HttpRequestSender* activerequest;
};


#endif // CLIENT_H
