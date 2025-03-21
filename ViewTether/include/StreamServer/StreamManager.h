#pragma once
#include <QObject>
#include <QString>


class StreamManager : public  QObject {
	Q_OBJECT

public:

private:
	QString serverIp;
	QString serverPort;

public:
	//StreamManager();
	//~StreamManager();

	//QString getServerIp();
	//QString getServerPort();


private:

};
