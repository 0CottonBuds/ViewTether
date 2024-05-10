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
	void readWhenReady(QTcpSocket* socket);
	void sendDataToAll();

private:
	QTimer* sendTimer;
	QTcpServer *server;
	vector<QTcpSocket*> m_connections;
};
