#include "insert.h"
#include "../../managers/managers.h"

Statement::ExecState Insert::exec()
{
	RecordManager *manager = RecordManager::getInstance();
	manager->insert(tableName, values);
	return EXEC_SUCCESS;
}

QDebug operator<<(QDebug debug, const Insert& info)
{
	debug << "table name: " << info.tableName << endl;
	debug << "values: " << endl;
	QStringListIterator iter(info.values);
	while(iter.hasNext())
		debug << iter.next() << endl;
	return debug;
}
