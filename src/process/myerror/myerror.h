#ifndef MYERROR_H_
#define MYERROR_H_

#include <QString>
#include <QStringList>

class MyError
{
public:
	enum ErrorType
	{
		EMPTY_TEXT,
		UNKNOWN_STATEMENT,
		SYNTAX_ERROR,
		PRIMARY_KEY_ERROR,
		DATATYPE_ERROR,
		DUPLICATED_NAME,
		DATATYPE_OUTOFBOUND,
		UNKNOWN_COMPARE,
		CONDITION_INCOMPLETE,
		ATTR_OUTOFBOUND,
		TABLE_EXIST,
		TABLE_NOTFOUND,
		INSERT_ATTRINUM_ERROR,
		INSERT_DATATYPE_ERROR,
		INSERT_STRING_TOOLONG,
		NAME_TOOLONG,
		JOIN_ERROR,
		JOIN_ITSELT,
		ATTR_NOTFOUND,
		ATTR_AMBIGUOUS,
		ATTR_NOTFOUND_COND,
		ATTR_AMBIGUOUS_COND,
		COND_TYPEERROR,
		FILE_NOTFOUND,
		EXECFILE_ERROR,
		UNIQUE_DUPLICATE,
		ATTR_NOTUNIQUE,
		INDEX_DUPLICATENAME,
		INDEX_DUPLICATEATTR,
		INDEX_NOTFOUND
	};
private:
	const ErrorType errorType;
	int lineNumber;
	QString errorStatement;

	MyError* subError;

public:
	MyError(ErrorType errorType, int lineNum = 1, QString errorStatement = QString());
	MyError(const MyError& error);
	~MyError() { delete subError; }

	QString getErrorInfo();
	MyError* getSubError() const { return subError; }
	void setSubError(const MyError& error) { subError = new MyError(error); }
};

#endif
