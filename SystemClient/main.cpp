#include <QCoreApplication>
#include <QDebug>
#include <QTextStream>
#include <iostream>

#include "client.h"

using namespace std;

void printHelp() {
    qInfo() << "The following is the list of all supported commands:\n"
               " Login <username> <pass> -> prints counter ID \n"
               " Counter ID <CIN> -> prints counter ID of given account\n"
               " Pay Bill <counterID> -> pay bill\n"
               " Exit -> logout";

    qInfo() << "The following is the list of all special supported commands for admin only:\n"
               " List Bills -> prints Bills Database\n"
               " List Users -> prints Users Database\n"
               " Add Bill<counterID, billdata> \n"
               " Create a New User<userdata> \n"
               " Update a User<counterID, userdata> \n"
               " Delete a User<CIN> \n";
}

int main(int argc, char *argv[])
{
    QTcpSocket *socket = new QTcpSocket();
    socket->connectToHost("192.168.1.101", 8080);


    QCoreApplication a(argc, argv);

    Client client;

    bool isLoggedIn = false;
    QString role;
    QTextStream input(stdin);
    QTextStream output(stdout);

    qDebug() << "Welcome to our Electricity Billing System...";
    printHelp();
    input.flush();  // Ensuring previous input is cleared

    while(true){

    QString userInput = input.readLine();
        if (userInput.contains("login", Qt::CaseInsensitive)) {
            if (isLoggedIn) {
                qInfo() << "Already logged in...\n";
                break;
            }

            qDebug() << "Please choose who you are?";
            qDebug() << "\"User\" or \"Admin\"";
            QString userType = input.readLine().trimmed();
            role = userType;

            qInfo() << "Enter User name:";
            QString username = input.readLine().trimmed();

            qInfo() << "Enter Password:";
            QString password = input.readLine().trimmed();

            // Perform login logic here
            if (userType.contains("user", Qt::CaseInsensitive)) {
                client.Log_in(username, password, "standard");
            } else if (userType.contains("admin", Qt::CaseInsensitive)) {
                client.Log_in(username, password, "admin");
            }
            isLoggedIn = true;

        } else if (userInput.contains("Counter ID", Qt::CaseInsensitive)) {
            qInfo() << "Enter CIN number:";
            QString CIN = input.readLine().trimmed();
            client.Get_CounterID(CIN);

        } else if (userInput.contains("List Bills", Qt::CaseInsensitive)) {
            if (role.contains("user", Qt::CaseInsensitive)) {
                qInfo() << "Administrator privileges are required to proceed";
            } else if (role.contains("admin", Qt::CaseInsensitive)) {
                client.listBills();
            }


        } else if (userInput.contains("List Users", Qt::CaseInsensitive)) {
            if (role.contains("user", Qt::CaseInsensitive)) {
                qInfo() << "Administrator privileges are required to proceed";
            } else if (role.contains("admin", Qt::CaseInsensitive)) {
                client.listUsers();
            }

        } else if (userInput.contains("Add Bill", Qt::CaseInsensitive)) {
            if (role.contains("user", Qt::CaseInsensitive)) {
                qInfo() << "Administrator privileges are required to proceed";
            } else if (role.contains("admin", Qt::CaseInsensitive)) {
                qInfo() << "Enter counter ID:";
                QString counterID = input.readLine().trimmed();
                qInfo() << "Enter Bill data as a form of \n Amount:theamount,Year:theyear,Month:themonth";
                QString data = input.readLine().trimmed();
                client.Add_Bill(counterID,data);
            }

        } else if (userInput.contains("Pay Bill", Qt::CaseInsensitive)) {
            qInfo() << "Enter counter ID:";
            QString counterID = input.readLine().trimmed();
            client.Pay_Bill(counterID);

        } else if (userInput.contains("Delete user", Qt::CaseInsensitive)) {
            if (role.contains("user", Qt::CaseInsensitive)) {
                qInfo() << "Administrator privileges are required to proceed";
            } else if (role.contains("admin", Qt::CaseInsensitive)) {
                qInfo() << "Enter CIN number of the account you want to delete:";
                QString CIN = input.readLine().trimmed();
                client.Delete_User(CIN);
            }

        } else if (userInput.contains("Create user", Qt::CaseInsensitive)) {
            if (role.contains("user", Qt::CaseInsensitive)) {
                qInfo() << "Administrator privileges are required to proceed";
            } else if (role.contains("admin", Qt::CaseInsensitive)) {
                qInfo() << "Enter new User data as a form of \n username:yourname,password:yourpassword,email:youremail,counterID:yourcounterID,CIN:yourCIN";
                QString data = input.readLine().trimmed();
                client.Create_User(data);
            }

        } else if (userInput.contains("Update user", Qt::CaseInsensitive)) {
            if (role.contains("user", Qt::CaseInsensitive)) {
                qInfo() << "Administrator privileges are required to proceed";
            } else if (role.contains("admin", Qt::CaseInsensitive)) {
                qInfo() << "Enter the counter ID of the account you want to update:";
                QString counterID = input.readLine().trimmed();
                qInfo()<< "which data you want to update : 1.name \n2.Password\n3.email\n4.All of the above";
                QString datatype=input.readLine().trimmed();
                if(datatype.contains("name", Qt::CaseInsensitive))
                {
                    qInfo() << "Enter new User name as a form of \n username:yournewname";
                    QString data = input.readLine().trimmed();
                    client.Update_User(counterID,data);
                }
                else if(datatype.contains("password", Qt::CaseInsensitive))
                {
                    qInfo() << "Enter new password as a form of \n password:yourpassword";
                    QString data = input.readLine().trimmed();
                    client.Update_User(counterID,data);
                }
                else if(datatype.contains("email", Qt::CaseInsensitive))
                {
                    qInfo() << "Enter new email as a form of \n email:youremail";
                    QString data = input.readLine().trimmed();
                    client.Update_User(counterID,data);
                }
                else if(datatype.contains("All of the above", Qt::CaseInsensitive))
                {
                    qInfo() << "Enter new Data as a form of \n username:yournewname,password:yourpassword,email:youremail";
                    QString data = input.readLine().trimmed();
                    client.Update_User(counterID,data);
                }
            }

        } else if (userInput.contains("help", Qt::CaseInsensitive)) {
            printHelp();

        } else if (userInput.contains("exit", Qt::CaseInsensitive)) {
            qInfo() << "Electricity Billing System is Exiting now...";
            isLoggedIn = false;
            break;

        } else {
            qInfo() << "Invalid Command. Type 'help' to print the manual\n";

        }

    }

    return a.exec();

}


