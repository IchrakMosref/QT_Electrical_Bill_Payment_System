#ifndef DATABASE_H
#define DATABASE_H

#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QFile>
#include <QDebug>
#include <QByteArray>

QByteArray createDatabase(QJsonObject& database);


#endif // DATABASE_H
