#pragma once
#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <vector>

using namespace std;

class DisplayStreamServer : QObject {
	Q_OBJECT
public:
	explicit DisplayStreamServer(QObject* parent = 0);

signals:

public slots:
	void newConnection();
	void readyRead(QTcpSocket* socket);

private:
	QTcpServer *server;
	vector<QTcpSocket*> m_connections;
};
