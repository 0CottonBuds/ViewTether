#include "DisplayStreamServer.h"

DisplayStreamServer::DisplayStreamServer(QObject* parent) : QObject(parent)
{
	server = new QTcpServer(this);

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
    QTcpSocket* socket = server->nextPendingConnection();

    socket->write("Hello client\r\n");
    socket->write("You are now connected to Screen Capture server\r\n");

    connect(socket, &QTcpSocket::readyRead, this, &DisplayStreamServer:: readWhenReady);
    connect(socket, &QTcpSocket::disconnected, socket, &QTcpSocket::deleteLater);

    m_connections.push_back(socket);
}

void DisplayStreamServer::sendDataToAll(const char data) {

    for (QTcpSocket* socket : m_connections) {
        socket->write(&data);
        socket->waitForBytesWritten();
    }
    qDebug() << "Finished sending packets to all clients" << endl;
}

void DisplayStreamServer::readWhenReady() {

    QTcpSocket* socket = qobject_cast<QTcpSocket*> (sender());

	QString data;
    while (socket->bytesAvailable()) {
        data.append(socket->readAll());
    }
    qDebug() << "Client Response: " << data << endl;
}
