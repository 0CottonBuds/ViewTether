#pragma once
#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <vector>
#include <QTimer>

using namespace std;

class DisplayStreamServer : QObject {
	Q_OBJECT
public:
	explicit DisplayStreamServer(QObject* parent = 0);

public slots:
	void newConnection();
	void readWhenReady();
	void sendDataToClient();

private:
	QTimer* sendTimer;
	QTcpServer* server;
	QTcpSocket* client = nullptr;
};
