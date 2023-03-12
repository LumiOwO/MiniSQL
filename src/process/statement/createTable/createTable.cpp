#include "createTable.h"
#include "../../managers/managers.h"

Statement::ExecState CreateTable::exec()
{
	CatalogManager *manager = CatalogManager::getInstance();
	manager->createTable(tableName, attributes);
	return EXEC_SUCCESS;
}

QDebug operator<<(QDebug debug, const CreateTable& info)
{
	debug << "table name: ";
	debug << info.tableName << endl;

	debug << "attributes:" << endl;
	QVectorIterator<Attribute*> iter(info.attributes);
	while(iter.hasNext())
	{
		Attribute *attr = iter.next();
		debug << attr->getName() << attr->getDataType() << attr->isUnique() << endl;
	}
	return debug;
}
