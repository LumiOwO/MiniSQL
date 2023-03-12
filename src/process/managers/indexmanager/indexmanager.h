#ifndef INDEXMANAGER_H_
#define INDEXMANAGER_H_

#include "../../blocks/nodeBlock/nodeBlock.h"
#include <QString>

class IndexManager
{
// Singleton
private:
	IndexManager() {}
	static IndexManager *instance;
public:
	static IndexManager *getInstance();
	static void free();

	// create index
	void createIndex(QString indexName, QString tableName, QString attrName);
	// drop index
	void dropIndex(QString indexName);
	// drop index by table
	void dropIndex(QString tableName, QStringList uniqueAttrs);
	// check whether the index is already in the catalog
	bool hasIndex(QString indexName) const;
	// check whether the attribute is already has an index
	bool hasIndex(QString tableName, QString attrName) const;
	// get index name by attribute
	QString getIndexName(QString tableName, QString attrName) const;
	// get the root block of the index
	NodeBlock* getRoot(QString indexName) const;
	// get all index info to show
	QVector<QStringList> getAllIndexInfo() const;
};

#endif
