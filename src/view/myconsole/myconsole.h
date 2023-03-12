#ifndef MYCONSOLE_H
#define MYCONSOLE_H

#include <QString>
#include "../mytextfield/mytextfield.h"

class MyConsole
{
private:
	MyTextField *console;
public:
	MyConsole(QFont font);
	~MyConsole();
	MyTextField *getWidget();
	void printFilePath(QString path);
	void appendMessage(QString message);
	void appendHelpInfo();
	void appendRunningTime(int time_ms);
	void clearText();
};

#endif // MYCONSOLE_H
