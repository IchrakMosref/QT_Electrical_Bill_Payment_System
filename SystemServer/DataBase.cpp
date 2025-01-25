#include "DataBase.h"


QByteArray createDatabase(QJsonObject& database) {
    // Create initial users
    QJsonArray userArray;

    // User 1
    QJsonObject user1;
    user1["CIN"] = "1356987";
    user1["username"] = "yasser arafet";
    user1["password"] = "password123";
    user1["authority"] = "standard";
    user1["email"] = "yasser_arafet@example.com";
    user1["counterID"] = "E101";
    userArray.append(user1);

    // User 2
    QJsonObject user2;
    user2["CIN"] = "04568295";
    user2["username"] = "admin user";
    user2["password"] = "passwordadmin";
    user2["authority"] = "admin";
    user2["email"] = "admin_user@example.com";
    user2["counterID"] = "E102";
    userArray.append(user2);

    // User 3
    QJsonObject user3;
    user3["CIN"] = "14065798";
    user3["username"] = "idris mohammed";
    user3["password"] = "passwordabc";
    user3["authority"] = "standard";
    user3["email"] = "idris_mohammed@example.com";
    user3["counterID"] = "E103";
    userArray.append(user3);

    // Add the users array to the database object
    database["users"] = userArray;

    // Create initial bills
    QJsonArray billArray;

    QJsonObject bill1;
    bill1["counterID"] = "E101";
    bill1["amount"] = 75.50;
    bill1["year"] = "2024";
    bill1["month"] = "08";
    bill1["status"] = "unpaid";
    billArray.append(bill1);

    QJsonObject bill2;
    bill2["counterID"] = "E103";
    bill2["amount"] = 120.00;
    bill2["year"] = "2024";
    bill2["month"] = "08";
    bill2["status"] = "unpaid";
    billArray.append(bill2);

    // Add the bills array to the database object
    database["bills"] = billArray;

    // Create a JSON document from the database
    QJsonDocument jsonDocument(database);

    // Convert the JSON document to a string
    QString jsonString = jsonDocument.toJson();

    // Convert the JSON document to a QByteArray
    QByteArray jsonData = jsonDocument.toJson();

    // Print the JSON string
    qDebug() << jsonString;

    // Specify the file path to save the JSON data
    QString filePath = "DataBase.json";

    // Write the JSON data to the file
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "Failed to open file for writing.";
    }
    file.write(jsonData);
    file.close();

    qDebug() << "JSON data saved to file: " << filePath;

    return jsonData;
}

