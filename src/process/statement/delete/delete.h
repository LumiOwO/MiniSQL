#ifndef DELETE_H_
#define DELETE_H_

#include "../statement.h"
#include "../../condition/condition.h"
#include <QString>
#include <QVector>

class Delete: public Statement
{
private:
	QString tableName;
	QVector<Condition*> conditions;
public:
	Delete(QString tableName, QVector<Condition*> conditions):
		tableName(tableName), conditions(conditions) {}
	virtual ExecState exec();

	friend QDebug operator<<(QDebug debug, const Delete& info);
};


#endif
