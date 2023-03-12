#include "mylinenum.h"

MyLineNum::MyLineNum(QScrollBar *scrollBar, QFont font):
	scrollBar(scrollBar),
	lineNumbers(new MyTextField),
	lines(1),
	nowWidth(0)
{
	lineHighlightFormat.setForeground(QBrush(QColor(214, 197, 64)));

	font.setPixelSize(25);
	lineNumbers->setFont(font);

	QPalette palette;
	palette.setColor(QPalette::Base, QColor(64, 66, 68));
	palette.setColor(QPalette::Text, QColor(190, 192, 194));
	palette.setColor(QPalette::Highlight, QColor(64, 66, 68));
	palette.setColor(QPalette::HighlightedText, QColor(190, 192, 194));
	lineNumbers->setPalette(palette);

	lineNumbers->setReadOnly(true);
	lineNumbers->setEnabled(false);
	lineNumbers->setFocusPolicy(Qt::NoFocus);
	lineNumbers->setFrameShape(QFrame::NoFrame);
	lineNumbers->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	lineNumbers->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

	updateLineNumbers();
}

MyLineNum::~MyLineNum()
{
	delete lineNumbers;
}

MyTextField *MyLineNum::getWidget()
{
	return lineNumbers;
}

void MyLineNum::setScrollBar(QScrollBar *scrollBar)
{
	this->scrollBar = scrollBar;
}

void MyLineNum::setLines(int lines, int nowLine)
{
	if(lines != this->lines)
		this->lines = lines;
	updateLineNumbers(nowLine);
	updateScrollPosition();
}

void MyLineNum::updateLineNumbers(int nowLine)
{
	// set width
	int width;
	if(lines >= 100)
	{
		int cnt = 0;
		int temp = lines;
		while(temp > 0)
		{
			cnt ++;
			temp /= 10;
		}
		width = cnt + 1;
	}
	else
		width = 3;
	if(width != nowWidth)
	{
		nowWidth = width;
		lineNumbers->setMinimumWidth(UNIT_WIDTH * width);
		lineNumbers->setMaximumWidth(UNIT_WIDTH * width);
		lineNumbers->setPlainText("");
	}

	// update lines
	QString nowText = lineNumbers->toPlainText();
	int lastEnter = nowText.lastIndexOf("\n");
	int lastNum = nowText.mid(lastEnter+1).toInt();
	if(lastNum < lines)
	{
		ostringstream newLineNums;
		for(int i=lastNum+1; i<=lines; i++)
		{
			if(i != 1)
				newLineNums << endl;
			newLineNums << setw(width) << i;
		}
		nowText += newLineNums.str().data();
	}
	else if(lastNum > lines)
	{
		nowText = nowText.left(lines*(width+1));
		nowText.chop(1);
	}
	lineNumbers->setPlainText(nowText);

	// set nowline highlight
	QTextCursor cursor = lineNumbers->textCursor();
	cursor.beginEditBlock();

	cursor.setPosition(nowLine*(width+1));
	cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
	cursor.setCharFormat(lineHighlightFormat);

	cursor.endEditBlock();
}

void MyLineNum::updateScrollPosition()
{
	int now = scrollBar->value();
	lineNumbers->verticalScrollBar()->setSliderPosition(now);
}

