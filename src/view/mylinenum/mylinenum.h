#ifndef MYLINENUM_H
#define MYLINENUM_H

#include <QTextCursor>
#include <QScrollBar>
#include <iostream>
#include <iomanip>
#include <sstream>
#include "../mytextfield/mytextfield.h"

#define UNIT_WIDTH 19

using namespace std;

class MyLineNum
{
private:
	QScrollBar *scrollBar;
	MyTextField *lineNumbers;
	QTextCharFormat lineHighlightFormat;
	int lines;
	int nowWidth;
private:
	void updateLineNumbers(int nowLine = 0);
public:
	MyLineNum(QScrollBar *scrollBar, QFont font);
	~MyLineNum();
	MyTextField *getWidget();
	void updateScrollPosition();
	void setLines(int lines, int nowLine);
	void setScrollBar(QScrollBar *scrollBar);
};

#endif // MYLINENUM_H
