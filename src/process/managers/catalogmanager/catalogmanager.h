#ifndef CATALOGMANAGER_H_
#define CATALOGMANAGER_H_

#include <QString>
#include <QVector>

class Attribute;

class CatalogManager
{
// Singleton
private:
	CatalogManager() {}
	static CatalogManager *instance;
public:
	static CatalogManager *getInstance();
	static void free();

	// create table by given name and attributes
	void createTable(QString name, QVector<Attribute*> attributes);
	// drop table by given name
	void dropTable(QString name);
	// check whether the table is already in the catalog
	bool hasTable(QString name);
	// get all tables in the catalog
	QStringList getAllTables() const;
	// get all the attribute names of a table
	QStringList getAttrNames(QString tableName) const;
};

#endif
