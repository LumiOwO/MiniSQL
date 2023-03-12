#ifndef STATEMENT_H_
#define STATEMENT_H_

#include <QDebug>
#include <QString>

// base class
class Statement
{
private:
	QString code;
public:
	enum ExecState{
		EXEC_ERROR, EXEC_SUCCESS, EXEC_QUIT
	};
	virtual ~Statement();
	virtual ExecState exec() = 0;

	void setCode(QString code) { this->code = code; }
	QString getCode() { return code; }
};

#endif
