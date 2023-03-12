#include "createIndex.h"
#include "../../managers/managers.h"

Statement::ExecState CreateIndex::exec()
{
	IndexManager *indexManager = IndexManager::getInstance();
	indexManager->createIndex(indexName, tableName, attrName);
	return EXEC_SUCCESS;
}

QDebug operator<<(QDebug debug, const CreateIndex& info)
{
	debug << "index name: " << info.indexName << endl;
	debug << "table name: " << info.tableName << endl;
	debug << "attribute name: " << info.attrName << endl;
	return debug;
}
