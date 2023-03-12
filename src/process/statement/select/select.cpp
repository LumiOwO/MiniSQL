#include "select.h"
#include "../../managers/managers.h"

Statement::ExecState Select::exec()
{
	RecordManager *manager = RecordManager::getInstance();
	manager->select(select, from, conditions);
	return EXEC_SUCCESS;
}

QDebug operator<<(QDebug debug, const Select& info)
{
	debug << "select: " << info.select << endl;
	debug << "from: " << info.from << endl;
	debug << "conditions: " << endl;
	QVectorIterator<Condition*> iter(info.conditions);
	while(iter.hasNext())
	{
		Condition* cond = iter.next();
		debug << cond->getAttrName() << cond->getCompare() << cond->getValue() << endl;
	}
	return debug;
}
