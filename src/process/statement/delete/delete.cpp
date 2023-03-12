#include "delete.h"
#include "../../managers/managers.h"

Statement::ExecState Delete::exec()
{
	RecordManager *manager = RecordManager::getInstance();
	manager->deLete(tableName, conditions);
	return EXEC_SUCCESS;
}

QDebug operator<<(QDebug debug, const Delete& info)
{
	debug << "delete table: " << info.tableName << endl;
	debug << "conditions: " << endl;
	QVectorIterator<Condition*> iter(info.conditions);
	while(iter.hasNext())
	{
		Condition* cond = iter.next();
		debug << cond->getAttrName() << cond->getCompare() << cond->getValue() << endl;
	}
	return debug;
}
