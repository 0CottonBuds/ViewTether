#pragma once

#include <qobject.h>

class DriverHelper : public QObject{
	Q_OBJECT

public:
	// check if amyuni USB Mobile Monitor Virtual Display driver is installed
	bool isAmyuniInstalled();

	// install amyuni drivers
	void installAmyuni();

	// uninstall amyuni drivers
	void uninstallAmyuni();

	// add a virtual monitor with amyuni drivers
	void addVirtualScreen();

	// remove a virtual monitor with amyuni drivers
	void removeVirtualScreen();

signals:
	void virtualScreenModified();
};
