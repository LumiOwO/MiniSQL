#ifndef MYHIGHLIGHTER_H
#define MYHIGHLIGHTER_H

#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QRegExp>
#include <QDebug>
#include <QVector>
#include <QStringList>

class HighlightStyle
{
public:
	QRegExp regExp;
	QTextCharFormat charFormat;
	HighlightStyle() {}
	HighlightStyle(QRegExp regExp, QTextCharFormat charFormat):
		regExp(regExp), charFormat(charFormat) {}
};

class MyHighlighter: public QSyntaxHighlighter
{
private:
	QVector<HighlightStyle> styles;

	// multi paragraph patterns
	QTextCharFormat commentBlockFormat;
	QRegExp blockStart;
	QRegExp blockEnd;

	QTextCharFormat stringFormat;
	QRegExp stringStart;
	QRegExp stringEnd;
public:
	MyHighlighter(QTextDocument *parent);
	void highlightBlock(const QString &text);
};

#endif // MYHIGHLIGHTER_H
