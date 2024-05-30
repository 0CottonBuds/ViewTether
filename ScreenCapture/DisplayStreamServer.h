#pragma once
#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <vector>
#include <QTimer>
#include <Windows.h>

using namespace std;

class DisplayStreamServer : public QObject {
	Q_OBJECT
public:
	explicit DisplayStreamServer(QObject* parent = 0);

	QString getServerIp();
	QString getServerPort();

public slots:
	void newConnection();
	void readWhenReady();
	void sendDataToClient(shared_ptr<UCHAR> pData);

signals:
	void connected();
	void disconnected();

private:
	QTcpServer* server;
	QTcpSocket* client = nullptr;

	QString serverIp;
	QString serverPort = "9999";
};
