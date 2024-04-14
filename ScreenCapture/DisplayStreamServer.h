#pragma once
#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>

class DisplayStreamServer : QObject {
	Q_OBJECT
public:
	explicit DisplayStreamServer(QObject* parent = 0);

signals:

public slots:
	void newConnection();

private:
	QTcpServer *server;
};
