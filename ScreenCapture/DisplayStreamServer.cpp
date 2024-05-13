#include "DisplayStreamServer.h"

DisplayStreamServer::DisplayStreamServer(QObject* parent) : QObject(parent)
{
    server = new QTcpServer();

    // whenever a user connects, it will emit signal
    connect(server, SIGNAL(newConnection()), this, SLOT(newConnection()));

    if(!server->listen(QHostAddress::Any, 9999))
    {
		QHostAddress serverAddr = server->serverAddress();
        qDebug() << "Server could not start";
        qDebug() << serverAddr.toString();
    }
    else
    {
		QHostAddress serverAddr = server->serverAddress();
        qDebug() << "Server started!";
        qDebug() << serverAddr.toString();
    }
}

void DisplayStreamServer::newConnection()
{
    // if there is a client connected
    if (client != nullptr) {
		QTcpSocket* socket = server->nextPendingConnection();
        socket->write("Sorry there is currently a connected client to the server!");
        socket->waitForBytesWritten();
        socket->disconnectFromHost();
        return;
    }

    client = server->nextPendingConnection();

    client->write("Hello client\r\n");
    client->write("You are now connected to Screen Capture server\r\n");

    connect(client, SIGNAL(readyRead()), this, SLOT(readWhenReady()));

    // handle client disconnect
    connect(client, &QTcpSocket::disconnected, client, &QTcpSocket::deleteLater);
    connect(client, &QTcpSocket::disconnected, client, [this] {client = nullptr; });
}

void DisplayStreamServer::sendDataToClient() {
    if (client == nullptr) 
		return;

	client->write("Tick Tock!!");
	client->waitForBytesWritten();
}

void DisplayStreamServer::readWhenReady() {
	QString data;
    while (client->bytesAvailable()) {
        data.append(client->readAll());
    }
    qDebug() << "Client Response: " << data << endl;
}
