#include "DisplayStreamServer.h"

DisplayStreamServer::DisplayStreamServer(QObject* parent) : QObject(parent)
{
	server = new QTcpServer(this);
    sendTimer = new QTimer();
    sendTimer->setInterval(1000 / 30);

    // whenever a user connects, it will emit signal
    connect(server, SIGNAL(newConnection()), this, SLOT(newConnection()));
    connect(sendTimer, SIGNAL(timeout()), this, SLOT(sendDataToAll()));
    sendTimer->start();

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

    connect(socket, &QTcpSocket::readyRead, this, [this, socket]{readWhenReady(socket);});
    connect(socket, &QTcpSocket::disconnected, socket, &QTcpSocket::deleteLater);

    m_connections.push_back(socket);
}

void DisplayStreamServer::sendDataToAll() {

    for (int i = 0; i < m_connections.size(); i++) {
        try {
            QTcpSocket* socket = m_connections[i];
			socket->write("Tick Tock!!");
			socket->waitForBytesWritten();
        }
        catch (exception e) {
            qDebug() << "Error at socket " << i << ".";
            qDebug() << e.what();
        }
	}
	qDebug() << "Finished sending packets to all clients" << endl;
    
}

void DisplayStreamServer::readWhenReady(QTcpSocket* socket) {
	QString data;
    while (socket->bytesAvailable()) {
        data.append(socket->readAll());
    }
    qDebug() << "Client Response: " << data << endl;
}
