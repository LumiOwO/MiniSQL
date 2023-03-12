#include "myhighlighter.h"

MyHighlighter::MyHighlighter(QTextDocument *parent):
	QSyntaxHighlighter(parent)
{
	// dec bin oct hex
	QTextCharFormat numberFormat;
	numberFormat.setForeground(QBrush(QColor(172, 128, 255)));
	QStringList numbers;
	numbers << "0(?:d|D)[0-9]+" << "0(?:b|B)[0-1]+"
			<< "0(?:o|O)[0-7]+" << "0(?:x|X)[0-9a-fA-F]+"
			<< "[0-9]+'(?:d|D)[0-9]+" << "[0-9]+'(?:b|B)[0-1]+"
			<< "[0-9]+'(?:o|O)[0-7]+" << "[0-9]+'(?:h|H)[0-9a-fA-F]+"
			<< "\\b[0-9]+\\b";
	for(int i=0; i<numbers.size(); i++)
	{
		QRegExp number(numbers[i]);
		styles.push_back(HighlightStyle(number, numberFormat));
	}

	// keywords;
	QTextCharFormat keywordsFormat;
	keywordsFormat.setForeground(QBrush(QColor(234, 36, 107)));
	QStringList keywords;
	keywords << "\\bselect\\b" << "\\bfrom\\b" << "\\bwhere\\b"
			 << "\\border by\\b" << "\\bcreate\\b" << "\\btable\\b"
			 << "\\bprimary key\\b" << "\\bdrop\\b" << "\\bindex\\b"
			 << "\\bon\\b" << "\\bdelete from\\b" << "\\binsert into\\b"
			 << "\\bvalues\\b" << "\\bquit\\b" << "\\bexecfile\\b"
			 << "\\bunique\\b" << "\\band\\b";
	for(int i=0; i<keywords.size(); i++)
	{
		QRegExp keyword(keywords[i], Qt::CaseInsensitive);
		styles.push_back(HighlightStyle(keyword, keywordsFormat));
	}

	// variable type
	QTextCharFormat specialWordsFormat;
	specialWordsFormat.setForeground(QBrush(QColor(103, 216, 239)));
	specialWordsFormat.setFontItalic(true);
	QStringList specialWords;
	specialWords << "\\bint\\b" << "\\bchar\\b" << "\\bfloat\\b";
	for(int i=0; i<specialWords.size(); i++)
	{
		QRegExp specialWord(specialWords[i], Qt::CaseInsensitive);
		styles.push_back(HighlightStyle(specialWord, specialWordsFormat));
	}

	// block comment
	commentBlockFormat.setForeground(QBrush(QColor(116, 112, 93)));
	blockStart.setPattern("\\/\\*");
	blockEnd.setPattern("\\*\\/");

	// string
	stringFormat.setForeground(QBrush(QColor(231, 219, 116)));
	stringStart.setPattern("\"|\'");
	stringEnd.setPattern("\"|\'");
}

void MyHighlighter::highlightBlock(const QString &text)
{
	// styles in one line
	for(int i=0; i<styles.size(); i++)
	{
		int pos = 0;
		while((pos = styles[i].regExp.indexIn(text, pos)) != -1)
		{
			int len = styles[i].regExp.matchedLength();
			setFormat(pos, len, styles[i].charFormat);
			pos += len;
		}
	}

	// block state:
	enum STATE
	{
		NOT_IN = 0,			// 00: not in any area
		IN_COMMENT = 1,		// 01: in comment area
		IN_STRING = 2		// 10: in string area
	};
	setCurrentBlockState(NOT_IN);
	int start;

	// string
	int jumpFlag;
	if(previousBlockState() == IN_STRING)
	{
		jumpFlag = 0;
		start = 0;
	}
	else
	{
		jumpFlag = 1;
		start = -1;
		while((start = stringStart.indexIn(text, start+1)) != -1)
			if(!(start > 0 && ((text[start-1] == '\\') |
							(text.left(start).contains("//")) |
							(text.left(start).contains("#" ))  ) ))
				break;
	}
	while(start != -1)
	{
		int end = start + jumpFlag - 1;
		while((end = stringEnd.indexIn(text, end+1)) != -1)
			if(!(end > 0 && ((text[end-1] == '\\') |
							(text.left(end).contains("//")) |
							(text.left(end).contains("#" ))  ) ))
				break;

		int stringLength;
		if(end == -1)
		{
			setCurrentBlockState(IN_STRING);
			stringLength = text.length() - start;
		}
		else
		{
			jumpFlag = 1;
			stringLength = end - start + stringEnd.matchedLength();
		}
		setFormat(start, stringLength, stringFormat);
		start += stringLength - 1;
		while((start = stringStart.indexIn(text, start+1)) != -1)
			if(!(start > 0 && ((text[start-1] == '\\') |
							(text.left(start).contains("//")) |
							(text.left(start).contains("#" ))  ) ))
				break;
	}

	// line comment
	QTextCharFormat commentFormat;
	commentFormat.setForeground(QBrush(QColor(116, 112, 93)));
	QRegExp comment("(?:#|//)[^\\n]*");
	int pos = 0;
	while((pos = comment.indexIn(text, pos)) != -1)
	{
		int len = comment.matchedLength();
		setFormat(pos, len, commentFormat);
		pos += len;
	}

	// block comment
	if(previousBlockState() == IN_COMMENT)
		start = 0;
	else
		start = blockStart.indexIn(text);
	while(start != -1)
	{
		int end = blockEnd.indexIn(text);
		int commentLength;
		if(end == -1)
		{
			setCurrentBlockState(IN_COMMENT);
			commentLength = text.length() - start;
		}
		else
			commentLength = end - start + blockEnd.matchedLength();
		setFormat(start, commentLength, commentBlockFormat);
		start = blockStart.indexIn(text, start + commentLength);
	}
}
