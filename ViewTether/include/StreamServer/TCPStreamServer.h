#pragma once
#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <vector>
#include <QTimer>
#include <Windows.h>
#include <QHostInfo>
#include <QHostAddress>
#include <QNetworkInterface>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
}

using namespace std;

class DisplayStreamServer : public QObject {
	Q_OBJECT
public:
	explicit DisplayStreamServer(QObject* parent = 0);

	QString getServerIp();
	QString getServerPort();

public slots:
	void run();
	void read();
	void write(AVPacket* packet);

signals:
	void connected();
	void disconnected();
	void initializationFinished();

private slots:
	void newConnection();
	QByteArray serializeAvPacket(AVPacket* packet);

private:
	QTcpServer* server;
	QTcpSocket* client = nullptr;

	QString serverHostName;
	QString serverIp;
	QString serverPort = "9999";

	void getHostInformation();
};
