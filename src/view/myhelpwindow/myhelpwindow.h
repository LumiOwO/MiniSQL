#ifndef MYHELPWINDOW_H
#define MYHELPWINDOW_H

#include <QMainWindow>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QScrollBar>
#include <QPalette>
#include <QGridLayout>
#include <QTextStream>
#include <QString>
#include <QFile>
#include "../mytextfield/mytextfield.h"

#define HELPWIN_WIDTH	1100
#define HELPWIN_HEIGHT	600

class MyHelpWindow
{
private:
	QMainWindow *helpWindow;
	MyTextField *helpTextField;
	QDialogButtonBox *button;
public:
	MyHelpWindow(QFont font);
	~MyHelpWindow();
	void show();
};

#endif
