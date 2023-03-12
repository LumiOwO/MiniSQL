#include "dropTable.h"
#include "../../managers/managers.h"

Statement::ExecState DropTable::exec()
{
	CatalogManager *manager = CatalogManager::getInstance();
	manager->dropTable(tableName);
	return EXEC_SUCCESS;
}

QDebug operator<<(QDebug debug, const DropTable& info)
{
	debug << "drop table: " << info.tableName << endl;
	return debug;
}
