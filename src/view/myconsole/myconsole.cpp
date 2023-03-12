#include "myconsole.h"

MyConsole::MyConsole(QFont font):
	console(new MyTextField)
{
	font.setPixelSize(25);
	console->setFont(font);

	QPalette palette;
	palette.setColor(QPalette::Base, QColor(46, 47, 48));
	palette.setColor(QPalette::Text, QColor(235, 235, 235));
	console->setPalette(palette);

	console->setReadOnly(true);
}

MyConsole::~MyConsole()
{
	delete console;
}

MyTextField *MyConsole::getWidget()
{
	return console;
}

void MyConsole::printFilePath(QString path)
{
	console->setPlainText(path + ":\n");
}

void MyConsole::appendMessage(QString message)
{
	console->setPlainText(console->toPlainText() + message + '\n');
}

void MyConsole::appendHelpInfo()
{
	QString help = "提示: \n"
				   "\t1. 只支持单属性主键的定义;\n"
				   "\t2. 在定义主键之前，该属性必须已经在表中定义过;\n"
				   "\t3. where语句中，只支持与逻辑，即用and连接多个条件。\n";
	appendMessage(help);
}

void MyConsole::appendRunningTime(int time_ms)
{
	appendMessage("Running Time: " + QString::number(double(time_ms) / 1000) + " s.");
}

void MyConsole::clearText()
{
	console->setPlainText("");
	console->repaint();
}
