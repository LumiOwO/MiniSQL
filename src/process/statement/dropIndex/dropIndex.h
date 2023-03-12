#ifndef DROPINDEX_H_
#define DROPINDEX_H_

#include "../statement.h"
#include <QString>

class DropIndex: public Statement
{
private:
	QString indexName;
public:
	DropIndex(QString indexName):
		indexName(indexName) {}
	virtual ExecState exec();

	friend QDebug operator<<(QDebug debug, const DropIndex& info);
};


#endif
