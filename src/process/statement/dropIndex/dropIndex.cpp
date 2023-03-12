#include "dropIndex.h"
#include "../../managers/managers.h"

Statement::ExecState DropIndex::exec()
{
	IndexManager *indexManager = IndexManager::getInstance();
	indexManager->dropIndex(indexName);
	return EXEC_SUCCESS;
}

QDebug operator<<(QDebug debug, const DropIndex& info)
{
	debug << "drop index: " << info.indexName << endl;
	return debug;
}
