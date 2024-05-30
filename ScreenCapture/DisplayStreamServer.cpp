#include "DisplayStreamServer.h"

DisplayStreamServer::DisplayStreamServer(QObject* parent) : QObject(parent)
{
    server = new QTcpServer();

    // whenever a user connects, it will emit signal
    connect(server, SIGNAL(newConnection()), this, SLOT(newConnection()));
    connect(server, SIGNAL(newConnection()), this, SIGNAL(connected()));

    if(!server->listen(QHostAddress::Any, 9999))
    {
		QHostAddress serverAddr = server->serverAddress();
        serverIp = serverAddr.toString();
        qDebug() << "Server could not start";
        qDebug() << serverIp;
    }
    else
    {
		QHostAddress serverAddr = server->serverAddress();
        serverIp = serverAddr.toString();
        qDebug() << "Server started!";
        qDebug() << serverIp;
    }
}

QString DisplayStreamServer::getServerIp()
{
    if (serverIp == "0.0.0.0") 
        return "localhost (only local host)";
    return serverIp;
}

QString DisplayStreamServer::getServerPort()
{
    return serverPort;
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
    connect(client, &QTcpSocket::disconnected, client, [this] {client = nullptr;});
    connect(client, &QTcpSocket::disconnected, this, &DisplayStreamServer::disconnected);
}

void DisplayStreamServer::sendDataToClient(shared_ptr<UCHAR> pData) {
    if (client == nullptr) {
        qDebug() << "There is no client connected";
		return;
    }

	client->write((char*) pData.get());
	client->waitForBytesWritten();
}

void DisplayStreamServer::readWhenReady() {
	QString data;
    while (client->bytesAvailable()) {
        data.append(client->readAll());
    }
    qDebug() << "Client Response: " << data;
}
