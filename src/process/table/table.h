#ifndef _TABLE_H_
#define _TABLE_H_

#include <QStringList>
#include <QVector>
#include <QDebug>
#include "../condition/condition.h"
#include "../blocks/tableBlock/tableBlock.h"

class Table
{
private:
	QStringList attrNames;
	QVector<Attribute::DataType> dataTypes;
	QVector<QStringList> records;
public:
	Table() {}

	// getters
	QStringList getAttrNames() const { return attrNames; }
	QVector<QStringList> getRecords() const { return records; }

	// operation
	void join(QString tableName, QSet<QPair<int, int>>* recordsIdx, bool withTableName);
	Table subTable(QStringList select, QVector<Condition*> conditions) const;

	// copy data
	void copy(const Table& table);
	void copy(const Table* table);

	// debug
	friend QDebug operator<<(QDebug debug, const Table& table);
	friend QDebug operator<<(QDebug debug, const Table* table);
};

#endif
