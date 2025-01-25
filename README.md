# Electrical_Bill_Payment_System

This Electrical Bill Payment System is developed using the Qt/C++ framework. The client-server interface implements the following request/response pairs: User Authentication, View Bills, Pay Bill, and View Users. The system is distributed with one daemon application (Server) running on a Raspberry Pi Zero and the client application on Windows. Communication between the two machines is handled via the TCP Sockets mechanism. JSON is used as the database format to store users and bills data. The server can handle multiple requests from multiple clients using threads, with each request being executed in a separate thread until it is handled.

### Key Features:

1. User Authentication: Allows users to log in and register.
2. View Bills: Allows administrators to access the bills database.
3. Pay Bill: Allows users to pay their outstanding bills.
4. View Users: Allows administrators to access and manage user information.
5. Retrieve Electricity Counter ID: Allows users to look up their electricity counter ID using their CIN number.
