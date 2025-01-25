#include "client.h"

Client::Client():activerequest(nullptr)
{
    // Constructor implementation
    socket.connectToHost("192.168.1.101", 8080);
}

void Client::setHttpRequestSender(HttpRequestSender* sender)
{
    activerequest = sender;
    activerequest->SetSocket(&socket); // to set the socket of the request
}

void Client::Log_in(QString username ,QString pass,QString authority)
{
    qDebug()<<"Log in Function";

    PostRequestSender* loginRequest = new PostRequestSender();
    setHttpRequestSender(loginRequest);

    // Create a JSON object
    QJsonObject jsonObject;
    jsonObject["username"] =  username;
    jsonObject["password"] =  pass;
    jsonObject["authority"] = authority;
    QJsonDocument jsonDocument(jsonObject);

    // Convert the JSON object to a QByteArray
    QByteArray jsonData = jsonDocument.toJson();

    loginRequest->sendRequest("post","http://192.168.1.101:8080/postlogin",jsonData);

    delete loginRequest;
}

void Client::Pay_Bill(QString counterID)
{
    qDebug()<<"Pay Bill Function";

    PostRequestSender* PayBillRequest = new PostRequestSender();
    setHttpRequestSender(PayBillRequest);

    // Create a JSON object
    QJsonObject jsonObject;
    jsonObject["counterID"] =  counterID;
    QJsonDocument jsonDocument(jsonObject);

    // Convert the JSON object to a QByteArray
    QByteArray jsonData = jsonDocument.toJson();

    PayBillRequest->sendRequest("post","http://192.168.1.101:8080/postpaybill",jsonData);

    delete PayBillRequest;
}

void Client::Create_User(QString User_Data)
{
    qDebug()<<"Create User Function";

    PostRequestSender* CreateUserRequest = new PostRequestSender();
    setHttpRequestSender(CreateUserRequest);

    int usernameIndex = User_Data.indexOf("username:") + QString("username:").length();
    int usernameEndIndex = User_Data.indexOf(",", usernameIndex);
    QString username = User_Data.mid(usernameIndex, usernameEndIndex - usernameIndex);

    // Extracting the password
    int passwordIndex = User_Data.indexOf("password:") + QString("password:").length();
    int passwordEndIndex = User_Data.indexOf(",", passwordIndex);
    QString password = User_Data.mid(passwordIndex, passwordEndIndex - passwordIndex);

    // Extracting the email
    int emailIndex = User_Data.indexOf("email:") + QString("email:").length();
    int emailEndIndex = User_Data.indexOf(",", emailIndex);
    QString email = User_Data.mid(emailIndex, emailEndIndex - emailIndex);

    // Extracting the counterID
    int counterIDIndex = User_Data.indexOf("counterID:") + QString("counterID:").length();
    int counterIDEndIndex = User_Data.indexOf(",", counterIDIndex);
    QString counterID = User_Data.mid(counterIDIndex, counterIDEndIndex - counterIDIndex);

    // Extracting the account number
    int CINIndex = User_Data.indexOf("CIN:") + QString("CIN:").length();
    QString CIN = User_Data.mid(CINIndex);

    qDebug()<<username;
    qDebug()<<password;
    qDebug()<<CIN;

    // Create a JSON object
    QJsonObject jsonObject;
    jsonObject["username"] = username;
    jsonObject["password"] =  password;
    jsonObject["email"]= email;
    jsonObject["counterID"]= counterID;
    jsonObject["CIN"]= CIN;

    QJsonDocument jsonDocument(jsonObject);

    // Convert the JSON object to a QByteArray
    QByteArray jsonData = jsonDocument.toJson();

    CreateUserRequest->sendRequest("post","http://192.168.1.101:8080/postcreateuser",jsonData);

    delete CreateUserRequest;
}

void Client::Delete_User(QString CIN)
{
    qDebug()<<"Delete User Function";

    DeleteRequestSender* DeleteUserRequest = new DeleteRequestSender();
    setHttpRequestSender(DeleteUserRequest);

    // Create a JSON object
    QJsonObject jsonObject;
    jsonObject["CIN"]= CIN;
    QJsonDocument jsonDocument(jsonObject);

    // Convert the JSON object to a QByteArray
    QByteArray jsonData = jsonDocument.toJson();

    DeleteUserRequest->sendRequest("delete","http://192.168.1.101:8080/deleteuser",jsonData);

    delete DeleteUserRequest;
}

void Client::Update_User(QString counterID,QString User_Data)
{
    qDebug()<<"Update User Function";

    PostRequestSender* UpdateUserRequest = new PostRequestSender();
    setHttpRequestSender(UpdateUserRequest);

    // Create a JSON object
    QJsonObject jsonObject;

    if(User_Data.contains("username:")&&User_Data.contains("password:")&&User_Data.contains("email:"))
    {
        int usernameIndex = User_Data.indexOf("username:") + QString("username:").length();
        int usernameEndIndex = User_Data.indexOf(",", usernameIndex);
        QString username = User_Data.mid(usernameIndex, usernameEndIndex - usernameIndex);

        //extracting the password
        int passwordIndex = User_Data.indexOf("password:") + QString("password:").length();
        int passwordEndIndex = User_Data.indexOf(",", passwordIndex);
        QString password = User_Data.mid(passwordIndex, passwordEndIndex - passwordIndex);

        // Extracting the email
        int emailIndex = User_Data.indexOf("email:") + QString("email:").length();
        QString email = User_Data.mid(emailIndex);

        qDebug()<<username;
        qDebug()<<password;

        jsonObject["username"] = username;
        jsonObject["password"] =  password;
        jsonObject["email"] =  email;
    }

    else if(User_Data.contains("username:"))
    {
        int usernameIndex = User_Data.indexOf("username:") + QString("username:").length();
        QString username = User_Data.mid(usernameIndex);
        qDebug()<<username;
        jsonObject["username"] = username;
    }
    else if(User_Data.contains("password:"))
    {
        // Extracting the password
        int passwordIndex = User_Data.indexOf("password:") + QString("password:").length();
        QString password = User_Data.mid(passwordIndex);
        qDebug()<<password;
        jsonObject["password"] = password;
    }
    else if(User_Data.contains("email:"))
    {
        // Extracting the password
        int emailIndex = User_Data.indexOf("email:") + QString("email:").length();
        QString email = User_Data.mid(emailIndex);
        qDebug()<<email;
        jsonObject["email"] = email;
    }
    jsonObject["counterID"]=counterID;
    QJsonDocument jsonDocument(jsonObject);

    // Convert the JSON object to a QByteArray
    QByteArray jsonData = jsonDocument.toJson();

    UpdateUserRequest->sendRequest("post","http://192.168.1.101:8080/postupdateuser",jsonData);

    delete UpdateUserRequest;
}

void Client::Get_CounterID(QString CIN)
{
    qDebug()<<"Get Account Number Function";

    PostRequestSender* GetCounterIDRequest = new PostRequestSender();
    setHttpRequestSender(GetCounterIDRequest);

    // Create a JSON object
    QJsonObject jsonObject;
    jsonObject["CIN"] = CIN;
    QJsonDocument jsonDocument(jsonObject);

    // Convert the JSON object to a QByteArray
    QByteArray jsonData = jsonDocument.toJson();

    GetCounterIDRequest->sendRequest("post","http://192.168.1.101:8080/postgetcounterID",jsonData);

    delete GetCounterIDRequest;
}

void Client::listUsers()
{
    qDebug()<<"List Users Function";

    GetRequestSender* listUsersRequest = new GetRequestSender();
    setHttpRequestSender(listUsersRequest);

    listUsersRequest->sendRequest("get","http://192.168.1.101:8080/getuserslist","");

    delete listUsersRequest;
}

void Client::listBills()
{
    qDebug()<<"List Bills Function";

    GetRequestSender* listBillsRequest = new GetRequestSender();
    setHttpRequestSender(listBillsRequest);

    listBillsRequest->sendRequest("get","http://192.168.1.101:8080/getbillslist","");

    delete listBillsRequest;
}

Client::~Client()
{
    delete activerequest;
}

void Client::Add_Bill(QString counterID, QString Bill_Data)
{
    qDebug() << "Add Bill Function";

    PostRequestSender* AddBillRequest = new PostRequestSender();
    setHttpRequestSender(AddBillRequest);

    // Extracting the amount as a float
    int amountIndex = Bill_Data.indexOf("Amount:") + QString("Amount:").length();
    int amountEndIndex = Bill_Data.indexOf(",", amountIndex);
    float amount = Bill_Data.mid(amountIndex, amountEndIndex - amountIndex).toFloat();

    // Extracting the year as an integer
    int yearIndex = Bill_Data.indexOf("Year:") + QString("Year:").length();
    int yearEndIndex = Bill_Data.indexOf(",", yearIndex);
    int year = Bill_Data.mid(yearIndex, yearEndIndex - yearIndex).toInt();

    // Extracting the month as an integer
    int monthIndex = Bill_Data.indexOf("Month:") + QString("Month:").length();
    int month = Bill_Data.mid(monthIndex).toInt();

    qDebug() << "Amount:" << amount;
    qDebug() << "Year:" << year;
    qDebug() << "Month:" << month;

    // Create a JSON object
    QJsonObject jsonObject;
    jsonObject["counterID"] = counterID;
    jsonObject["Amount"] = amount;  // Storing amount as a float
    jsonObject["Year"] = year;      // Storing year as an integer
    jsonObject["Month"] = month;    // Storing month as an integer
    jsonObject["Status"] = "unpaid";

    QJsonDocument jsonDocument(jsonObject);

    // Convert the JSON object to a QByteArray
    QByteArray jsonData = jsonDocument.toJson();

    // Send the request
    AddBillRequest->sendRequest("post", "http://192.168.1.101:8080/postaddbill", jsonData);

    delete AddBillRequest;
}
