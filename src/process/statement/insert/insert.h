#ifndef INSERT_H_
#define INSERT_H_

#include "../statement.h"
#include <QStringList>

class Insert: public Statement
{
private:
	QString tableName;
	QStringList values;
public:
	Insert(QString tableName, QStringList values):
		tableName(tableName), values(values) {}
	virtual ExecState exec();

	friend QDebug operator<<(QDebug debug, const Insert& info);
};


#endif
