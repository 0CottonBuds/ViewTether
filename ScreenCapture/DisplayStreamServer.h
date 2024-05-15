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

public slots:
	void newConnection();
	void readWhenReady();
	void sendDataToClient(UCHAR* pData);

private:
	QTcpServer* server;
	QTcpSocket* client = nullptr;
};
