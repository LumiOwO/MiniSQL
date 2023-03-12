#ifndef SELECT_H_
#define SELECT_H_

#include "../statement.h"
#include "../../condition/condition.h"
#include <QStringList>
#include <QVector>

class Select: public Statement
{
private:
	QStringList select;
	QStringList from;
	QVector<Condition*> conditions;
public:
	Select(QStringList select, QStringList from, QVector<Condition*> conditions):
		select(select), from(from), conditions(conditions) {}
	virtual ExecState exec();

	friend QDebug operator<<(QDebug debug, const Select& info);
};


#endif
