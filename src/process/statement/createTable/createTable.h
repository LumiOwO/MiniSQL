#ifndef CREATETABLE_H_
#define CREATETABLE_H_

#include "../statement.h"
#include "../../attribute/attribute.h"
#include <QString>
#include <QVector>

class CreateTable: public Statement
{
private:
	QString tableName;
	QVector<Attribute*> attributes;
public:
	CreateTable(QString tableName, QVector<Attribute*> attributes):
		tableName(tableName), attributes(attributes) {}
	virtual ExecState exec();

	friend QDebug operator<<(QDebug debug, const CreateTable& info);
};


#endif
