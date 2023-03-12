#include "mycodetext.h"

MyCodeText::MyCodeText(QFont font):
	textEdit(new MyTextField)
{
	font.setPixelSize(25);
	textEdit->setFont(font);

	QPalette palette;
	palette.setColor(QPalette::Base, QColor(46, 47, 48));
	palette.setColor(QPalette::Text, QColor(235, 235, 235));
	textEdit->setPalette(palette);

	textEdit->setStyleSheet(
		"QPlainTextEdit"
		"{"
			"selection-color: rgb(46, 47, 48);"
			"selection-background-color: rgb(255, 232, 147);"
		"}"
	);

	textEdit->setFrameShape(QFrame::NoFrame);
	textEdit->setLineWrapMode(QPlainTextEdit::NoWrap);
	textEdit->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

MyCodeText::~MyCodeText()
{
	delete textEdit;
}

MyTextField *MyCodeText::getWidget()
{
	return textEdit;
}

int MyCodeText::getLines()
{
	return textEdit->document()->lineCount();
}

QString MyCodeText::getCodeText()
{
	return textEdit->toPlainText();
}
