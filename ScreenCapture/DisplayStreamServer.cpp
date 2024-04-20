#include "DisplayStreamServer.h"

DisplayStreamServer::DisplayStreamServer(QObject* parent) : QObject(parent)
{
	server = new QTcpServer(this);

    // whenever a user connects, it will emit signal
    connect(server, SIGNAL(newConnection()), this, SLOT(newConnection()));
	QHostAddress serverAddr = server->serverAddress();

    if(!server->listen(QHostAddress::Any, 9999))
    {
        qDebug() << "Server could not start";
        qDebug() << serverAddr.toString();
    }
    else
    {
        qDebug() << "Server started!";
        qDebug() << serverAddr.toString();
    }
}

void DisplayStreamServer::newConnection()
{
    // need to grab the socket
    QTcpSocket* socket = server->nextPendingConnection();

    socket->write("Hello client\r\n");
    socket->write("How are you doin I am you are connected to the local host server");

    socket->waitForBytesWritten(3000);

    socket->waitForReadyRead();

    char* data = nullptr;
    qint64 responseCode = socket->read(data, 30000);

    if (responseCode == -1) {
        qDebug() << "An error has occurred" << endl;
    }
    else if (data == nullptr) {
        qDebug() << "User didn't give any data back" << endl;
    }
    else {
        qDebug() << "User response: " << data << endl;
    }

    socket->close();
}
