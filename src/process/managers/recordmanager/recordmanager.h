#ifndef RECORDMANAGER_H_
#define RECORDMANAGER_H_

#include <QStringList>
#include "../../table/table.h"

class RecordManager
{
// Singleton
private:
	RecordManager(): viewTable(new Table()) {}
	~RecordManager() { delete viewTable; }
	static RecordManager *instance;
public:
	static RecordManager *getInstance();
	static void free();

private:
	Table* viewTable;
public:
	// set data of view table
	void setViewTable(const Table& table) { viewTable->copy(table); }
	// get the header of view table
	QStringList getViewHeader() const { return viewTable->getAttrNames(); }
	// get the records to show
	QVector<QStringList> getViewRecords() const { return viewTable->getRecords(); }
	// get all values of an attribute
	QVector<QPair<QString, QPair<int, int>>> getAllValues(QString tableName, QString attrName) const;

	// insert a record into the table
	void insert(QString tableName, QStringList values);
	// select the attributes from the tables
	void select(QStringList select, QStringList from, QVector<Condition*> conditions);
	// delete records from the table
	void deLete(QString from, QVector<Condition*> conditions);

private:
	// check table names
	void checkName(const QStringList &from);
	void checkName(QString from) { checkName(QStringList({from})); }
	// check attributes and conditions
	void checkAttr(QStringList& select, QStringList& from, QVector<Condition*>& conditions);
	void checkAttr(QString from, QVector<Condition*>& conditions){
		QStringList select, fromL({from}); checkAttr(select, fromL, conditions);
	}
	// count the occur times in the list
	QStringList inWhichTable(const QString &attr, const QVector<QStringList> &attrList, QStringList from);
	// compact all the blocks to make sure only the last block has empty space
	void compact(QString tableName);
};

#endif
