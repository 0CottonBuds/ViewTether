#include "DisplayStreamServer.h"

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/opt.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
}


DisplayStreamServer::DisplayStreamServer(QObject* parent) : QObject(parent)
{
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

void DisplayStreamServer::run()
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

void DisplayStreamServer::newConnection()
{
    QByteArray msg;
    QDataStream msgStream(&msg, QIODevice::WriteOnly);

    // if there is a client connected
    if (client != nullptr) {
        msgStream << QString("msg").toUtf8();
        msgStream << QString("Sorry there is currently a connected client to the server");

		QTcpSocket* socket = server->nextPendingConnection();
        socket->write(msg);
        socket->waitForBytesWritten();
        socket->disconnectFromHost();
        return;
    }

    msgStream << QString("msg").toUtf8();
    msgStream << QString("Connected to Screen Capture Server").toUtf8();

    client = server->nextPendingConnection();
    client->write(msg);

    connect(client, SIGNAL(readyRead()), this, SLOT(readWhenReady()));

    // handle client disconnect
    connect(client, &QTcpSocket::disconnected, client, &QTcpSocket::deleteLater);
    connect(client, &QTcpSocket::disconnected, client, [this] {client = nullptr;});
    connect(client, &QTcpSocket::disconnected, this, &DisplayStreamServer::disconnected);
}

void DisplayStreamServer::sendDataToClient(AVPacket* packet) {
    if (client == nullptr) {
		av_packet_unref(packet);
		av_packet_free(&packet);
        return;
    }

    QByteArray serializedPacket = serializeAvPacket(packet);

	client->write(serializedPacket);
	client->waitForBytesWritten();

    av_packet_unref(packet);
    av_packet_free(&packet);
}

QByteArray DisplayStreamServer::serializeAvPacket(AVPacket* packet)
{
    QByteArray byteArray;
    QDataStream stream(&byteArray, QIODevice::WriteOnly);

    stream << QString("pkt").toUtf8();
    stream << static_cast<qint32>(packet->size);
    stream.writeRawData(reinterpret_cast<const char*>(packet->data), packet->size);

    return byteArray;
}

void DisplayStreamServer::readWhenReady() {
	QString data;
    while (client->bytesAvailable()) {
        data.append(client->readAll());
    }
    qDebug() << "Client Response: " << data;
}
