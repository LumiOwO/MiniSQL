#ifndef CREATEINDEX_H_
#define CREATEINDEX_H_

#include "../statement.h"
#include <QString>

class CreateIndex: public Statement
{
private:
	QString indexName;
	QString tableName;
	QString attrName;
public:
	CreateIndex(QString indexName, QString tableName, QString attrName):
		indexName(indexName), tableName(tableName), attrName(attrName) {}
	virtual ExecState exec();

	friend QDebug operator<<(QDebug debug, const CreateIndex& info);
};


#endif
