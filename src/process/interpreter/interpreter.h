#ifndef INTERPRETER_H_
#define INTERPRETER_H_

#include <QString>
#include <QVector>
#include "../statement/statement.h"

class Condition;

class Interpreter
{
public:
	Interpreter();
	~Interpreter();

	// parse the code text
	QVector<Statement*> parse(const QString& codeText);

	// get the line number of certain statement
	static int getLineNum(const QString& text, int StateNum);

private:
	bool equals(const QString& a, const QString& b){
		return QString::compare(a, b, Qt::CaseInsensitive) == 0;
	}

	// remove the useless text in the head and tail
	bool chopBothEnds(QString& code, QString headExp, QString endExp);
	// remove and return the first word in a code
	QString removeFirst(QString& code);
	// parse conditions
	QVector<Condition*> parseConditions(QString code);

	// get statement of certain type
	Statement* getCreateTable(QString code);
	Statement* getCreateIndex(QString code);
	Statement* getDropTable(QString code);
	Statement* getDropIndex(QString code);
	Statement* getSelect(QString code);
	Statement* getDelete(QString code);
	Statement* getInsert(QString code);
	Statement* getQuit(QString code);
	Statement* getExecFile(QString code);
};

#endif
