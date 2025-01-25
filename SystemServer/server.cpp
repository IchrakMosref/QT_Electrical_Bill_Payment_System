#include "server.h"
#include "DataBase.h"
#include "thread.h"

struct ClientData
{
    QString name;
    QString authority;
    QString counterID;
};

struct BillData
{
    QString amount;
    QString year;
    QString month;
};

QByteArray Data_Base;
QJsonObject database;

QMap<QTcpSocket *,ClientData> clientmap;
QMap<QTcpSocket *,BillData> billmap;


MyServer::MyServer(QObject *parent)
{
    Data_Base=createDatabase(database);

    if(!listen(QHostAddress::Any, 8080))
    {
        qDebug() << "Server could not start!";
    }
    else
    {
        qDebug() << "Server started!";
    }
}

void MyServer::incomingConnection(qintptr socketDescriptor) {

    qDebug() << socketDescriptor << " Connecting...";
    qDebug()<<"new connection";
    Thread *worker = new Thread(socketDescriptor);
    QThread* thread = new QThread;

    worker->moveToThread(thread);

    QTcpSocket *clientSocket = new QTcpSocket(this);
    clientSocket->setSocketDescriptor(socketDescriptor);

    connect(clientSocket, &QTcpSocket::readyRead, this, &MyServer::ReadRequest);
    connect(clientSocket, &QTcpSocket::disconnected, clientSocket, &QTcpSocket::deleteLater);
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));

    thread->start();
}

void MyServer::ReadRequest()
{

    QTcpSocket *socket = qobject_cast<QTcpSocket*>(sender());

    qDebug() << socket ;

    QByteArray requestData = socket->readAll();
    qDebug() << "Client request: " << requestData;

    QString requeststring(requestData);

    if(requeststring.startsWith("GET"))
    {
        if(requeststring.contains("/getuserslist")){
            processgetRequestlistusers(socket);
        }
        else if(requeststring.contains("/getbillslist")){
            processgetRequestlistbills(socket);
        }
    }
    else if(requeststring.startsWith("POST"))
    {
        if(requeststring.contains("/postlogin")){
            processPostRequestlogin(socket,requestData);
        }
        else if(requeststring.contains("/postgetcounterID")){
            processgetRequestgetcounterID(socket,requestData);
        }
        else if(requeststring.contains("/postpaybill"))
        {
            processPostRequestpaybill(socket, requestData);
        }
        else if(requeststring.contains("/postcreateuser"))
        {
            processPostRequestcreateuser(socket,requestData,database);
        }
        else if(requeststring.contains("/postupdateuser"))
        {
            processPostRequestupdateuser(socket,requestData,database);
        }
        else if(requeststring.contains("/postupdateuser"))
        {
            processPostRequestaddbill(socket,requestData,database);
        }
    }
    else if(requeststring.startsWith("PUT"))
    {
        QByteArray response = "HTTP/1.0 200 OK\r\n\r\n";
        response+="Hello client this is a PUT request";
        socket->write(response);
    }
    else if(requeststring.startsWith("DELETE"))
    {
        if(requeststring.contains("/deleteuser"))
        {
            processDeleteRequestdeleteuser(socket,requestData,database);
        }
    }
}

/*******************************************************************************************************************************/

void MyServer::processgetRequestlistusers(QTcpSocket* socket)
{
    qDebug() << "processgetRequestlistusers";

    QByteArray response;

    // Check if the client is logged in
    if (!clientmap.contains(socket)) {
        response = "HTTP/1.0 200 OK\r\n\r\n";
        response += "Not Logged in Before, Please Log in First";
        socket->write(response);
        socket->flush();
        socket->waitForBytesWritten();
        return;
    }

    // Check if the client has admin authority
    QString autho = clientmap.value(socket).authority;

    if (autho != "admin") {
        response = "HTTP/1.0 200 OK\r\n\r\n";
        response += "Not Authorized";
        socket->write(response);
        socket->flush();
        socket->waitForBytesWritten();
        return;
    }

    // Specify the file path to read the JSON data from
    QString filePath = "DataBase.json";

    // Create a file object
    QFile file(filePath);

    // Open the file in read-only mode
    if (!file.open(QIODevice::ReadOnly)) {
        response = "HTTP/1.0 200 OK\r\n\r\n";
        response += "Database Error";
        socket->write(response);
        socket->flush();
        socket->waitForBytesWritten();
        return;
    }

    // Read the JSON data from the file
    QByteArray jsonData = file.readAll();
    file.close();

    // Parse the JSON data
    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData);
    if (!jsonDoc.isObject()) {
        response = "HTTP/1.0 200 OK\r\n\r\n";
        response += "Invalid Data";
        socket->write(response);
        socket->flush();
        socket->waitForBytesWritten();
        return;
    }

    // Get the root JSON object and users array
    QJsonObject rootObject = jsonDoc.object();
    QJsonArray usersArray = rootObject.value("users").toArray();

    // Create a valid HTTP response with content-type header
    response = "HTTP/1.0 200 OK\r\n";
    response += "Content-Type: application/json\r\n\r\n";
    response += QJsonDocument(usersArray).toJson(); // Sending only the users array, not the entire database

    // Send the response back to the client
    socket->write(response);
    socket->flush();
    socket->waitForBytesWritten();
}

/*******************************************************************************************************************************/

void MyServer::processgetRequestlistbills(QTcpSocket* socket)
{
    qDebug() << "processgetRequestlistbills";

    QByteArray response;

    // Check if the client is logged in
    if (!clientmap.contains(socket)) {
        response = "HTTP/1.0 200 OK\r\n\r\n";
        response += "Not Logged in Before, Please Log in First";
        socket->write(response);
        socket->flush();
        socket->waitForBytesWritten();
        return;
    }

    // Get the user's authority and counterID
    QString authority = clientmap.value(socket).authority;
    QString userCounterID = clientmap.value(socket).counterID;

    // Specify the file path to read the JSON data from
    QString filePath = "DataBase.json";

    // Create a file object
    QFile file(filePath);

    // Open the file in read-only mode
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "Failed to open file for reading.";
        response = "HTTP/1.0 500 Internal Server Error\r\n\r\n";
        response += "Failed to open database file";
        socket->write(response);
        socket->flush();
        socket->waitForBytesWritten();
        return;
    }

    // Read the JSON data from the file
    QByteArray jsonData = file.readAll();
    file.close();

    // Parse the JSON data
    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData);
    if (!jsonDoc.isObject()) {
        qDebug() << "Invalid database format.";
        response = "HTTP/1.0 500 Internal Server Error\r\n\r\n";
        response += "Invalid database format";
        socket->write(response);
        socket->flush();
        socket->waitForBytesWritten();
        return;
    }

    // Get the root JSON object and bills array
    QJsonObject rootObject = jsonDoc.object();
    QJsonArray billsArray = rootObject.value("bills").toArray();

    // Create a JSON array to hold the filtered bills
    QJsonArray filteredBills;

    // Iterate through the bills
    for (const QJsonValue& value : billsArray) {
        QJsonObject billObject = value.toObject();

        // Check if the user is authorized to view the bill
        QString billCounterID = billObject.value("counterID").toString();

        // Admins can see all bills, standard users can only see their own bills
        if (authority == "admin" || billCounterID == userCounterID) {
            filteredBills.append(billObject);
        }
    }

    // Create the response with the filtered bills
    QJsonDocument filteredBillsDoc(filteredBills);
    QByteArray filteredBillsData = filteredBillsDoc.toJson();

    response = "HTTP/1.0 200 OK\r\n";
    response += "Content-Type: application/json\r\n\r\n";
    response += filteredBillsData;

    // Send the response back to the client
    socket->write(response);
    socket->flush();
    socket->waitForBytesWritten();
}

/*******************************************************************************************************************************/

void MyServer::processPostRequestcreateuser(QTcpSocket* socket, const QByteArray& requestData,QJsonObject& database)
{
    qDebug() << "processPostRequestcreateuser";

    QByteArray response;

    if (!clientmap.contains(socket)) {
        response = "HTTP/1.0 200 OK\r\n\r\n";
        response += "Not Logged in Before, Please Log in First";
        socket->write(response);
        socket->flush();
        socket->waitForBytesWritten();
        return;
    }

    QString autho = clientmap.value(socket).authority;
    qDebug() << autho;

    if (autho != "admin") {
        response = "HTTP/1.0 200 OK\r\n\r\n";
        response += "Not Authorized";
        socket->write(response);
        socket->flush();
        socket->waitForBytesWritten();
        return;
    }

    // Extract the JSON data from the request
    QByteArray jsonBytes = requestData.mid(requestData.indexOf('{'));

    // Parse the JSON data
    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonBytes);
    if (jsonDoc.isNull() || !jsonDoc.isObject()) {
        qDebug() << "Invalid JSON data";
        return;
    }

    QJsonObject jsonObject = jsonDoc.object();
    jsonObject["authority"]="standard";

    // Process the jsonObject as needed
    qDebug() << "Received POST request with data:";
    qDebug() << "username:" << jsonObject["username"].toString();
    qDebug() << "Password:" << jsonObject["password"].toString();
    qDebug() << "CIN:" << jsonObject["CIN"].toString();
    qDebug() << "email:" << jsonObject["email"].toString();
    qDebug() << "counterID:" << jsonObject["counterID"].toString();


    for(const auto& names:clientmap)
    {
        if(names.name==(jsonObject["username"].toString()))
        {
            response = "HTTP/1.0 200 OK\r\n\r\n";
            response += "can't create user, name is used before\nTry another name";
            socket->write(response);
            socket->flush();
            socket->waitForBytesWritten();
            return;

        }
    }


    QFile databaseFile("DataBase.json");
    if (!databaseFile.open(QIODevice::ReadWrite)) {
        qDebug() << "Failed to open database file";
        return;
    }

    QByteArray databaseData = databaseFile.readAll();
    databaseFile.close();

    QJsonDocument databaseDoc = QJsonDocument::fromJson(databaseData);
    if (!databaseDoc.isObject()) {
        qDebug() << "Invalid database file format";
        return;
    }

    QJsonObject databaseObject = databaseDoc.object();
    QJsonArray userArray = databaseObject.value("users").toArray();

    // Append the new user to the user array
    userArray.append(jsonObject);

    // Update the user array in the database
    databaseObject["users"] = userArray;

    // Create a JSON document from the updated database
    QJsonDocument jsonDocument(databaseObject);

    // Convert the JSON document to a QByteArray
    QByteArray jsonData = jsonDocument.toJson();

    // Write the modified JSON back to the file
    if (!databaseFile.open(QIODevice::WriteOnly | QIODevice::Truncate)) {  // Open the file in WriteOnly mode, truncating the existing content
        qDebug() << "Failed to open database file for writing";
        return;
    }
    databaseFile.write(jsonData);
    databaseFile.close();

    response = "HTTP/1.0 200 OK\r\n\r\n";
    response += "User Created Successfully";
    socket->write(response);
    socket->flush();
    socket->waitForBytesWritten();
    return;
}

/*******************************************************************************************************************************/

void MyServer::processPostRequestlogin(QTcpSocket* socket, const QByteArray& requestData)
{
    // Extract the JSON data from the request
    QByteArray jsonBytes = requestData.mid(requestData.indexOf('{'));

    QByteArray response;

    // Parse the JSON data
    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonBytes);
    if (!jsonDoc.isNull() && jsonDoc.isObject()) {
        QJsonObject jsonObject = jsonDoc.object();

        // Process the jsonObject
        qDebug() << "Received POST request with data:";
        qDebug() << "Name:" << jsonObject["username"].toString();
        qDebug() << "Password:" << jsonObject["password"].toString();
        qDebug() << "Authority:" << jsonObject["authority"].toString();

        QString name = jsonObject["username"].toString();
        QString password = jsonObject["password"].toString();
        QString authority = jsonObject["authority"].toString();

        // Use the searchInDatabase function to check if the user exists
        bool dataExists = searchInDatabase(name, password, authority);  // Implement this function below

        // Send a response to the client
        if (dataExists) {
            clientmap[socket].name = name;
            clientmap[socket].authority = authority;

            qDebug() << socket;
            qDebug() << clientmap[socket].name;
            qDebug() << clientmap[socket].authority;

            response = "HTTP/1.0 200 OK\r\n\r\n";
            response += "Logged in successfully";
        } else {
            response = "HTTP/1.0 200 OK\r\n\r\n";
            response += "User not found";
        }
    } else {
        // Handle invalid JSON
        response = "HTTP/1.0 200 OK\r\n\r\n";
        response += "Invalid JSON format";
    }

    // Send the response
    socket->write(response);
    socket->flush();
    socket->waitForBytesWritten();
}


/*******************************************************************************************************************************/

bool MyServer::searchInDatabase(const QString& name, QString& Password, QString& authority)
{
    QFile databaseFile("DataBase.json");
    if (!databaseFile.open(QIODevice::ReadOnly)) {
        qDebug() << "Failed to open database file";
        return false;
    }

    QByteArray databaseData = databaseFile.readAll();
    databaseFile.close();

    QJsonDocument jsonDoc = QJsonDocument::fromJson(databaseData);
    if (!jsonDoc.isObject()) {
        qDebug() << "Invalid database file format";
        return false;
    }

    QJsonObject jsonObject = jsonDoc.object();
    QJsonArray dataArray = jsonObject.value("users").toArray();

    // Iterate through the "users" array and check for matching username, password, and authority
    for (const QJsonValue& value : dataArray) {
        QJsonObject dataObject = value.toObject();
        QString dataName = dataObject.value("username").toString();
        QString dataPassword = dataObject.value("password").toString();
        QString dataAuthority = dataObject.value("authority").toString();

        // Case-sensitive comparison
        if (dataName == name && dataPassword == Password && dataAuthority == authority) {
            qDebug() << "User found in the database";
            return true; // User found in the database
        }
    }

    qDebug() << "User not found in the database";
    return false; // User not found
}


void MyServer::processDeleteRequestdeleteuser(QTcpSocket* socket, const QByteArray& requestData,QJsonObject& database)
{
    qDebug() << "processDeleteRequestdeleteuser";

    if (!clientmap.contains(socket)) {
        sendResponse(socket, "Not Logged in Before, Please Log in First");
        return;
    }

    QString autho = clientmap.value(socket).authority;
    qDebug() << autho;

    if (autho != "admin") {
        sendResponse(socket, "Not Authorized");
        return;
    }

    // Extract the JSON data from the request
    QByteArray jsonBytes = requestData.mid(requestData.indexOf('{'));

    // Parse the JSON data
    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonBytes);
    if (jsonDoc.isNull() || !jsonDoc.isObject()) {
        qDebug() << "Invalid JSON data";
        return;
    }

    QJsonObject jsonObject = jsonDoc.object();

    // Process the jsonObject as needed
    qDebug() << "Received DELETE request with data:";
    qDebug() << "CIN:" << jsonObject["CIN"].toString();

    QString CIN = jsonObject["CIN"].toString();

    QFile databaseFile("DataBase.json");
    if (!databaseFile.open(QIODevice::ReadWrite)) {
        qDebug() << "Failed to open database file";
        return;
    }

    QByteArray databaseData = databaseFile.readAll();
    databaseFile.close();

    QJsonDocument databaseDoc = QJsonDocument::fromJson(databaseData);
    if (!databaseDoc.isObject()) {
        qDebug() << "Invalid database file format";
        return;
    }

    QJsonObject databaseObject = databaseDoc.object();
    QJsonArray dataArray = databaseObject.value("users").toArray();

    bool DeleteDone = false;

    // Find and remove the user with the specified CIN
    for (int i = 0; i < dataArray.size(); ++i) {
        QJsonObject userObject = dataArray[i].toObject();
        if (userObject["CIN"].toString() == CIN && userObject["authority"] == "standard") {
            qDebug() << "Data found";
            dataArray.removeAt(i);
            // Update the user array in the database object
            databaseObject["users"] = dataArray;

            // Convert the updated database object back to JSON
            QJsonDocument updatedJsonDocument(databaseObject);
            QByteArray updatedJsonData = updatedJsonDocument.toJson();

            // Write the updated JSON data back to the file
            if (!databaseFile.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
                qDebug() << "Failed to open file for writing.";
                return;
            }

            databaseFile.write(updatedJsonData);
            databaseFile.close();
            sendResponse(socket, "Delete is done");
            DeleteDone = true;
            break;
        }
    }


    if (!DeleteDone) {
        qDebug() << "Delete cannot be done in the database";
        sendResponse(socket, "Delete cannot be done in the database");
    }

}

void MyServer::sendResponse(QTcpSocket* socket, const QString& responseMessage)
{
    QByteArray response = "HTTP/1.0 200 OK\r\n\r\n" + responseMessage.toUtf8();
    socket->write(response);
    socket->flush();
    socket->waitForBytesWritten();
}



void MyServer::processPostRequestaddbill(QTcpSocket* socket, const QByteArray& requestData, QJsonObject &database)
{
    qDebug() << "processPostRequestaddbill";

    QByteArray response;

    // Check if the client is logged in
    if (!clientmap.contains(socket)) {
        response = "HTTP/1.0 200 OK\r\n\r\n";
        response += "Not Logged in Before, Please Log in First";
        socket->write(response);
        socket->flush();
        socket->waitForBytesWritten();
        return;
    }

    // Get the user's authority
    QString autho = clientmap.value(socket).authority;
    qDebug() << autho;

    if (autho != "admin") {
        response = "HTTP/1.0 200 OK\r\n\r\n";
        response += "Not Authorized";
        socket->write(response);
        socket->flush();
        socket->waitForBytesWritten();
        return;
    }

    // Extract the JSON data from the request
    QByteArray jsonBytes = requestData.mid(requestData.indexOf('{'));

    // Parse the JSON data
    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonBytes);
    if (jsonDoc.isNull() || !jsonDoc.isObject()) {
        qDebug() << "Invalid JSON data";
        return;
    }

    QJsonObject newBillObject = jsonDoc.object();

    // Extract bill data from JSON
    QString counterID = newBillObject.value("counterID").toString();
    float amount = static_cast<float>(newBillObject.value("Amount").toDouble());
    QString year = newBillObject.value("Year").toString();
    QString month = newBillObject.value("Month").toString();
    QString status = newBillObject.value("Status").toString();

    // Read the database file
    QFile databaseFile("DataBase.json");
    if (!databaseFile.open(QIODevice::ReadOnly)) {
        response = "HTTP/1.0 200 OK\r\n\r\n";
        response += "Database Error";
        socket->write(response);
        socket->flush();
        socket->waitForBytesWritten();
        return;
    }

    QByteArray databaseData = databaseFile.readAll();
    databaseFile.close();

    // Parse the database
    QJsonDocument dbDoc = QJsonDocument::fromJson(databaseData);
    if (!dbDoc.isObject()) {
        response = "HTTP/1.0 200 OK\r\n\r\n";
        response += "Database Error";
        socket->write(response);
        socket->flush();
        socket->waitForBytesWritten();
        return;
    }

    // Add the new bill to the bills array
    QJsonObject rootObject = dbDoc.object();
    QJsonArray billsArray = rootObject.value("bills").toArray();

    QJsonObject billObject;
    billObject["counterID"] = counterID;
    billObject["Amount"] = amount;
    billObject["Year"] = year;
    billObject["Month"] = month;
    billObject["Status"] = status;

    billsArray.append(billObject); // Add the new bill

    // Update the root object with the modified bills array
    rootObject["bills"] = billsArray;

    // Save the updated database back to the file
    if (!databaseFile.open(QIODevice::WriteOnly)) {
        response = "HTTP/1.0 200 OK\r\n\r\n";
        response += "Database Error";
        socket->write(response);
        socket->flush();
        socket->waitForBytesWritten();
        return;
    }

    QJsonDocument updatedDoc(rootObject);
    databaseFile.write(updatedDoc.toJson());
    databaseFile.close();

    // Send a success response
    response = "HTTP/1.0 200 OK\r\n\r\n";
    response += "Bill added successfully";
    socket->write(response);
    socket->flush();
    socket->waitForBytesWritten();
}

void MyServer::processPostRequestpaybill(QTcpSocket* socket, const QByteArray& requestData)
{
    qDebug() << "processPostRequestpaybill";

    QByteArray response;

    // Extract the JSON data from the request
    QByteArray jsonBytes = requestData.mid(requestData.indexOf('{'));
    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonBytes);

    // Parse and validate JSON data
    if (jsonDoc.isNull() || !jsonDoc.isObject()) {
        response = "HTTP/1.0 200 OK\r\n\r\n";
        response += "Invalid Request";
        socket->write(response);
        socket->flush();
        socket->waitForBytesWritten();
        return;
    }

    QJsonObject jsonObject = jsonDoc.object();
    QString counterID = jsonObject.value("counterID").toString();

    // Validate if counterID is provided
    if (counterID.isEmpty()) {
        response = "HTTP/1.0 200 OK\r\n\r\n";
        response += "Invalid Data";
        socket->write(response);
        socket->flush();
        socket->waitForBytesWritten();
        return;
    }

    // Open the database file
    QString filePath = "DataBase.json";
    QFile file(filePath);

    if (!file.open(QIODevice::ReadOnly)) {
        response = "HTTP/1.0 200 OK\r\n\r\n";
        response += "Database Error";
        socket->write(response);
        socket->flush();
        socket->waitForBytesWritten();
        return;
    }

    // Read the JSON data from the file
    QByteArray databaseData = file.readAll();
    file.close();

    // Parse the database file
    QJsonDocument databaseDoc = QJsonDocument::fromJson(databaseData);
    if (!databaseDoc.isObject()) {
        response = "HTTP/1.0 200 OK\r\n\r\n";
        response += "Invalid Database";
        socket->write(response);
        socket->flush();
        socket->waitForBytesWritten();
        return;
    }

    QJsonObject databaseObject = databaseDoc.object();
    QJsonArray billsArray = databaseObject.value("bills").toArray();

    // Search for the matching counterID and mark the bill as paid
    bool billFound = false;
    for (int i = 0; i < billsArray.size(); ++i) {
        QJsonObject billObject = billsArray[i].toObject();
        if (billObject.value("counterID").toString() == counterID) {
            billObject["status"] = "paid";  // Update the bill status
            billsArray[i] = billObject;
            billFound = true;
            break;
        }
    }

    // If the bill was found and updated, save the changes to the database
    if (billFound) {
        databaseObject["bills"] = billsArray;

        // Save the updated database back to the file
        QFile saveFile(filePath);
        if (!saveFile.open(QIODevice::WriteOnly)) {
            response = "HTTP/1.0 200 OK\r\n\r\n";
            response += "Database Save Error";
            socket->write(response);
            socket->flush();
            socket->waitForBytesWritten();
            return;
        }

        QJsonDocument updatedDoc(databaseObject);
        saveFile.write(updatedDoc.toJson());
        saveFile.close();

        // Send success response
        response = "HTTP/1.0 200 OK\r\n\r\n";
        response += "Bill Paid Successfully";
    } else {
        // Send response if no matching bill was found
        response = "HTTP/1.0 200 OK\r\n\r\n";
        response += "Bill Not Found";
    }

    // Send the response back to the client
    socket->write(response);
    socket->flush();
    socket->waitForBytesWritten();
}

void MyServer::processgetRequestgetcounterID(QTcpSocket* socket, const QByteArray& requestData)
{
    qDebug() << "processgetRequestgetcounterID";

    QByteArray response;

    // Extract the JSON data from the request
    QByteArray jsonBytes = requestData.mid(requestData.indexOf('{'));
    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonBytes);

    // Parse and validate JSON data
    if (jsonDoc.isNull() || !jsonDoc.isObject()) {
        response = "HTTP/1.0 200 OK\r\n\r\n";
        response += "Invalid Request";
        socket->write(response);
        socket->flush();
        socket->waitForBytesWritten();
        return;
    }

    QJsonObject jsonObject = jsonDoc.object();
    QString CIN = jsonObject.value("CIN").toString();

    // Validate if CIN is provided
    if (CIN.isEmpty()) {
        response = "HTTP/1.0 200 OK\r\n\r\n";
        response += "Invalid Data";
        socket->write(response);
        socket->flush();
        socket->waitForBytesWritten();
        return;
    }

    // Open the database file
    QString filePath = "DataBase.json";
    QFile file(filePath);

    if (!file.open(QIODevice::ReadOnly)) {
        response = "HTTP/1.0 200 OK\r\n\r\n";
        response += "Database Error";
        socket->write(response);
        socket->flush();
        socket->waitForBytesWritten();
        return;
    }

    // Read the JSON data from the file
    QByteArray databaseData = file.readAll();
    file.close();

    // Parse the database file
    QJsonDocument databaseDoc = QJsonDocument::fromJson(databaseData);
    if (!databaseDoc.isObject()) {
        response = "HTTP/1.0 200 OK\r\n\r\n";
        response += "Invalid Database";
        socket->write(response);
        socket->flush();
        socket->waitForBytesWritten();
        return;
    }

    QJsonObject databaseObject = databaseDoc.object();
    QJsonArray usersArray = databaseObject.value("users").toArray();

    // Search for the matching CIN and retrieve the counterID
    QString counterID;
    bool userFound = false;
    for (const QJsonValue& value : usersArray) {
        QJsonObject userObject = value.toObject();
        if (userObject.value("CIN").toString() == CIN) {
            counterID = userObject.value("counterID").toString();
            userFound = true;
            break;
        }
    }

    // If the user is found, return the counterID
    if (userFound) {
        QJsonObject responseObject;
        responseObject["counterID"] = counterID;

        QJsonDocument responseDoc(responseObject);
        QByteArray jsonResponse = responseDoc.toJson();

        response = "HTTP/1.0 200 OK\r\n";
        response += "Content-Type: application/json\r\n\r\n";
        response += jsonResponse;
    } else {
        // If no user with the given CIN is found
        response = "HTTP/1.0 200 OK\r\n\r\n";
        response += "CIN Not Found";
    }

    // Send the response back to the client
    socket->write(response);
    socket->flush();
    socket->waitForBytesWritten();
}

void MyServer::processPostRequestupdateuser(QTcpSocket* socket, const QByteArray& requestData, QJsonObject& database)
{
    qDebug() << "processPostRequestupdateuser";

    // Check if the user is logged in and has admin authority
    if (!clientmap.contains(socket)) {
        sendResponse(socket, "Not Logged in Before, Please Log in First");
        return;
    }

    QString authority = clientmap.value(socket).authority;
    if (authority != "admin") {
        sendResponse(socket, "Not Authorized");
        return;
    }

    // Extract the JSON data from the request
    QByteArray jsonBytes = requestData.mid(requestData.indexOf('{'));
    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonBytes);

    // Parse and validate JSON data
    if (jsonDoc.isNull() || !jsonDoc.isObject()) {
        sendResponse(socket, "Invalid Request");
        return;
    }

    QJsonObject requestObject = jsonDoc.object();
    QString counterID = requestObject.value("counterID").toString();

    // Validate if counterID is provided
    if (counterID.isEmpty()) {
        sendResponse(socket, "Invalid Data");
        return;
    }

    // Search for the user with the given counterID in the database
    QJsonArray usersArray = database.value("users").toArray();
    bool userFound = false;

    for (int i = 0; i < usersArray.size(); ++i) {
        QJsonObject userObject = usersArray[i].toObject();

        // Match the counterID
        if (userObject.value("counterID").toString() == counterID) {
            // Update username if present
            if (requestObject.contains("username")) {
                QString newUsername = requestObject.value("username").toString();
                if (!newUsername.isEmpty()) {
                    userObject["username"] = newUsername;
                }
            }

            // Update password if present
            if (requestObject.contains("password")) {
                QString newPassword = requestObject.value("password").toString();
                if (!newPassword.isEmpty()) {
                    userObject["password"] = newPassword;
                }
            }

            // Update email if present
            if (requestObject.contains("email")) {
                QString newEmail = requestObject.value("email").toString();
                if (!newEmail.isEmpty()) {
                    userObject["email"] = newEmail;
                }
            }

            usersArray[i] = userObject;  // Update the user object in the array
            userFound = true;
            break;
        }
    }

    // Check if user was found and updated
    if (!userFound) {
        sendResponse(socket, "User Not Found");
        return;
    }

    // Update the database object
    database["users"] = usersArray;

    // Write the updated database back to the file
    QFile databaseFile("DataBase.json");
    if (!databaseFile.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        sendResponse(socket, "Failed to open database file for writing");
        return;
    }

    QJsonDocument updatedDatabaseDoc(database);
    databaseFile.write(updatedDatabaseDoc.toJson());
    databaseFile.close();

    // Respond with success
    sendResponse(socket, "User Updated");
}

