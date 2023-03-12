#include "../managers.h"
#include "../../myerror/myerror.h"
#include "../../blocks/blocks.h"

CatalogManager *CatalogManager::instance = nullptr;

CatalogManager *CatalogManager::getInstance()
{
	if(!instance)
		instance = new CatalogManager();
	return instance;
}

void CatalogManager::free()
{
	if(instance)
	{
		delete instance;
		instance = nullptr;
	}
}

void CatalogManager::createTable(QString name, QVector<Attribute*> attributes)
{
	if(hasTable(name))
		throw MyError::TABLE_EXIST;
	BufferManager* bufferManager = BufferManager::getInstance();

	int last = bufferManager->getBlockNums(CatalogBlock::NAME) - 1;
	CatalogBlock* catalog = (CatalogBlock*)bufferManager->getBlock(CatalogBlock::NAME, last);
	if(!catalog || !catalog->canInsert()) {
		catalog = new CatalogBlock(++last);
		catalog->setDirty(true);
		bufferManager->EnqueueBlock(catalog);
	}

	catalog->insertTable(name);
	TableBlock *table = new TableBlock(name, attributes, 0);
	table->setDirty(true);

//	qDebug() << catalog;
//	qDebug() << table;

	bufferManager->EnqueueBlock(table);
}

void CatalogManager::dropTable(QString name)
{
	if(!hasTable(name))
		throw MyError::TABLE_NOTFOUND;
	BufferManager* bufferManager = BufferManager::getInstance();

	// modify catalog
	int num = bufferManager->getBlockNums(CatalogBlock::NAME);
	for(int i=0; i<num; i++)
	{
		CatalogBlock *block = (CatalogBlock*)bufferManager->getBlock(CatalogBlock::NAME, i);
		if(block->hasTable(name))
		{
			block->dropTable(name);
			for(int j=i+1; j<num; j++)
			{
				CatalogBlock *nextBlock = (CatalogBlock*)bufferManager->getBlock(CatalogBlock::NAME, j);
				QString temp = nextBlock->getTableName(0);
				block->insertTable(temp);
				nextBlock->dropTable(temp);

				block = nextBlock;
			}
			break;
		}
	}

	// get unique attrbutes
	QStringList uniqueAttrs;
	TableBlock *table = (TableBlock*)bufferManager->getBlock(name, 0);
	for(int i=0; i<table->getAttrNum(); i++)
		if(table->isUniqueAt(i))
			uniqueAttrs << table->getAttrName(i);

	int tableBlockNums = bufferManager->getBlockNums(name);
	for(int i=0; i<tableBlockNums; i++)
	{
		table = (TableBlock*)bufferManager->getBlock(name, i);
		bufferManager->removeBlock(table);
	}
	bufferManager->deleteFile(name);

	// drop relative index
	IndexManager *indexManager = IndexManager::getInstance();
	indexManager->dropIndex(name, uniqueAttrs);
}

bool CatalogManager::hasTable(QString name)
{
	bool ret = false;
	BufferManager *bufferManager = BufferManager::getInstance();
	int num = bufferManager->getBlockNums(CatalogBlock::NAME);
	for(int i=0; i<num; i++)
	{
		CatalogBlock* catalog = (CatalogBlock*)bufferManager->getBlock(CatalogBlock::NAME, i);
		if(catalog->hasTable(name)) {
			ret = true;
			break;
		}
	}
	return ret;
}

QStringList CatalogManager::getAllTables() const
{
	QStringList ret;
	BufferManager *bufferManager = BufferManager::getInstance();
	int num = bufferManager->getBlockNums(CatalogBlock::NAME);
	for(int i=0; i<num; i++)
	{
		CatalogBlock* block = (CatalogBlock*)bufferManager->getBlock(CatalogBlock::NAME, i);
		for(int j=0; j<block->getTableCount(); j++)
			ret << block->getTableName(j);
	}

	return ret;
}

QStringList CatalogManager::getAttrNames(QString tableName) const
{
	QStringList ret;
	BufferManager *bufferManager = BufferManager::getInstance();

	TableBlock* block = (TableBlock*)bufferManager->getBlock(tableName, 0);
	QVector<Attribute*> attributes = block->getAttributes();
	QVectorIterator<Attribute*> iter(attributes);
	while(iter.hasNext())
	{
		Attribute* attribute = iter.next();
		ret << attribute->getName();
		delete attribute;
	}

	return ret;
}
