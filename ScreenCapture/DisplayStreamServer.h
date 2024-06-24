#pragma once
#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <vector>
#include <QTimer>
#include <Windows.h>

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
	void newConnection();
	void readWhenReady();
	void sendDataToClient(AVPacket* packet);

signals:
	void connected();
	void disconnected();

private:
	QTcpServer* server;
	QTcpSocket* client = nullptr;

	QString serverIp;
	QString serverPort = "9999";
};
