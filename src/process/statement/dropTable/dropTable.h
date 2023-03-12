#ifndef DROPTABLE_H_
#define DROPTABLE_H_

#include "../statement.h"
#include <QString>

class DropTable: public Statement
{
private:
	QString tableName;
public:
	DropTable(QString tableName):
		tableName(tableName) {}
	virtual ExecState exec();

	friend QDebug operator<<(QDebug debug, const DropTable& info);
};


#endif
