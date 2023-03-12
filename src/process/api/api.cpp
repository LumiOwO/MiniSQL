#include "api.h"
#include "../../view/mainwindow/mainwindow.h"
#include "../interpreter/interpreter.h"
#include "../myerror/myerror.h"

#include "../managers/managers.h"

API* API::instance = nullptr;

API* API::getInstance(MainWindow *window)
{
	if(!instance)
		instance = new API(window);

	// init managers
	BufferManager::getInstance();
	CatalogManager::getInstance();
	IndexManager::getInstance();
	RecordManager::getInstance();

	return instance;
}

void API::free()
{
	if(instance)
	{
		delete instance;
		instance = nullptr;
	}

	// free managers
	BufferManager::free();
	CatalogManager::free();
	IndexManager::free();
	RecordManager::free();
}

void API::execSQL(const QString& codeText)
{
	// clear view table
	RecordManager::getInstance()->setViewTable(Table());
	// interpret
	QVector<Statement*> result;
	Interpreter *interpreter = new Interpreter();
	try {
		result = interpreter->parse(codeText);
	} catch (MyError e) {
		delete interpreter;
		throw std::move(e);
	}
	delete interpreter;

	// exec SQL statements
	for(int i=0; i<result.size(); i++)
	{
		QString nowCode = result[i]->getCode();
		try {
			Statement::ExecState state = result[i]->exec();
			if(state == Statement::EXEC_QUIT)
				window->close();

		} catch (MyError::ErrorType type) {
			// free memory
			QVectorIterator<Statement*> iter(result);
			while(iter.hasNext())
				delete iter.next();

			throw MyError(type, Interpreter::getLineNum(codeText, i), nowCode);
		} catch (MyError e) {
			// exec file error

			// free memory
			QVectorIterator<Statement*> iter(result);
			while(iter.hasNext())
				delete iter.next();

			MyError error(MyError::EXECFILE_ERROR, Interpreter::getLineNum(codeText, i), nowCode);
			error.setSubError(e);
			throw error;
		}
	}
	QVectorIterator<Statement*> iter(result);
	while(iter.hasNext())
		delete iter.next();
}

void API::storePages()
{
	BufferManager::getInstance()->storePages();
}

QStringList API::getAllTables() const
{
	return CatalogManager::getInstance()->getAllTables();
}

QStringList API::getAttrNames(QString tableName) const
{
	return CatalogManager::getInstance()->getAttrNames(tableName);
}

QStringList API::getViewHeader() const
{
	return RecordManager::getInstance()->getViewHeader();
}

QVector<QStringList> API::getViewRecords() const
{
	return RecordManager::getInstance()->getViewRecords();
}

QVector<QStringList> API::getAllIndexInfo() const
{
	return IndexManager::getInstance()->getAllIndexInfo();
}
