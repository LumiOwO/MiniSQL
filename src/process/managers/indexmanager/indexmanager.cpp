#include "../managers.h"
#include "../../blocks/blocks.h"
#include "../../myerror/myerror.h"
#include "../../BPlusTree/BPlusTree.h"

IndexManager *IndexManager::instance = nullptr;

IndexManager *IndexManager::getInstance()
{
	if(!instance)
		instance = new IndexManager();
	return instance;
}

void IndexManager::free()
{
	if(instance)
	{
		delete instance;
		instance = nullptr;
	}
}

bool IndexManager::hasIndex(QString indexName) const
{
	bool ret = false;
	BufferManager *bufferManager = BufferManager::getInstance();
	int num = bufferManager->getBlockNums(IndexCatalogBlock::NAME);
	for(int i=0; i<num; i++)
	{
		IndexCatalogBlock* catalog = (IndexCatalogBlock*)bufferManager->getBlock(IndexCatalogBlock::NAME, i);
		if(catalog->hasIndex(indexName)) {
			ret = true;
			break;
		}
	}
	return ret;
}

bool IndexManager::hasIndex(QString tableName, QString attrName) const
{
	bool ret = false;
	BufferManager *bufferManager = BufferManager::getInstance();
	int num = bufferManager->getBlockNums(IndexCatalogBlock::NAME);
	for(int i=0; i<num; i++)
	{
		IndexCatalogBlock* catalog = (IndexCatalogBlock*)bufferManager->getBlock(IndexCatalogBlock::NAME, i);
		if(catalog->hasIndex(tableName, attrName)) {
			ret = true;
			break;
		}
	}
	return ret;
}

QString IndexManager::getIndexName(QString tableName, QString attrName) const
{
	QString ret;
	BufferManager *bufferManager = BufferManager::getInstance();
	int num = bufferManager->getBlockNums(IndexCatalogBlock::NAME);
	for(int i=0; i<num; i++)
	{
		IndexCatalogBlock* catalog = (IndexCatalogBlock*)bufferManager->getBlock(IndexCatalogBlock::NAME, i);
		if(catalog->hasIndex(tableName, attrName)) {
			ret = catalog->getIndexName(tableName, attrName);
			break;
		}
	}
	return ret;
}

NodeBlock* IndexManager::getRoot(QString indexName) const
{
	NodeBlock* ret = nullptr;
	BufferManager *bufferManager = BufferManager::getInstance();
	int num = bufferManager->getBlockNums(indexName);
	for(int i=0; i<num; i++)
	{
		NodeBlock* block = (NodeBlock*)bufferManager->getBlock(indexName, i);
		if(block->getParentNo() < 0) {
			ret = block;
			break;
		}
	}
	return ret;
}

void IndexManager::createIndex(QString indexName, QString tableName, QString attrName)
{
	// get managers
	BufferManager *bufferManager = BufferManager::getInstance();
	CatalogManager *catalogManager = CatalogManager::getInstance();
	RecordManager *recordManager = RecordManager::getInstance();

	// check table name
	if(!catalogManager->hasTable(tableName))
		throw MyError::TABLE_NOTFOUND;
	// check attribute
	TableBlock* tableBlock = (TableBlock*)bufferManager->getBlock(tableName, 0);
	int attrNo = tableBlock->getAttrNo(attrName);
	if(attrNo < 0)
		throw MyError::ATTR_NOTFOUND;
	if(!tableBlock->isUniqueAt(attrNo))
		throw MyError::ATTR_NOTUNIQUE;
	// check index name
	if(hasIndex(indexName))
		throw MyError::INDEX_DUPLICATENAME;
	if(hasIndex(tableName, attrName))
		throw MyError::INDEX_DUPLICATEATTR;

	// add into index catalog
	int last = bufferManager->getBlockNums(IndexCatalogBlock::NAME) - 1;
	IndexCatalogBlock* catalog = (IndexCatalogBlock*)bufferManager->getBlock(IndexCatalogBlock::NAME, last);
	if(!catalog || !catalog->canInsert()) {
		catalog = new IndexCatalogBlock(++last);
		catalog->setDirty(true);
		bufferManager->EnqueueBlock(catalog);
	}
	catalog->insertIndex(indexName, tableName, attrName);
//	qDebug() << catalog;

	// create index file
	LeafBlock* root = new LeafBlock(indexName, 0, tableBlock->getDataType(attrNo));
	BPlusTree tree(root);
	QVector<QPair<QString, QPair<int, int>>> values = recordManager->getAllValues(tableName, attrName);
//	qDebug() << values;
	tree.insert(values);
}

void IndexManager::dropIndex(QString indexName)
{
	if(!hasIndex(indexName))
		throw MyError::INDEX_NOTFOUND;
	BufferManager* bufferManager = BufferManager::getInstance();

	int num = bufferManager->getBlockNums(IndexCatalogBlock::NAME);
	for(int i=0; i<num; i++)
	{
		IndexCatalogBlock *block = (IndexCatalogBlock*)bufferManager->getBlock(IndexCatalogBlock::NAME, i);
		if(block->hasIndex(indexName))
		{
			block->dropIndex(indexName);
			for(int j=i+1; j<num; j++)
			{
				IndexCatalogBlock *nextBlock = (IndexCatalogBlock*)bufferManager->getBlock(IndexCatalogBlock::NAME, j);
				QString tempIndexName = nextBlock->getIndexName(0);
				QString tempTableName = nextBlock->getTableName(0);
				QString tempAttrName = nextBlock->getAttrName(0);
				block->insertIndex(tempIndexName, tempTableName, tempAttrName);
				nextBlock->dropIndex(tempIndexName);

				block = nextBlock;
			}
			break;
		}
	}
	int indexBlockNums = bufferManager->getBlockNums(indexName);
	for(int i=0; i<indexBlockNums; i++)
	{
		NodeBlock* index = (NodeBlock*)bufferManager->getBlock(indexName, i);
		bufferManager->removeBlock(index);
	}
	bufferManager->deleteFile(indexName);
}

void IndexManager::dropIndex(QString tableName, QStringList uniqueAttrs)
{
	QStringListIterator iter(uniqueAttrs);
	while(iter.hasNext())
	{
		QString indexName = getIndexName(tableName, iter.next());
		if(!indexName.isEmpty())
			dropIndex(indexName);
	}
}

QVector<QStringList> IndexManager::getAllIndexInfo() const
{
	QVector<QStringList> ret;
	BufferManager *bufferManager = BufferManager::getInstance();
	int num = bufferManager->getBlockNums(IndexCatalogBlock::NAME);
	for(int i=0; i<num; i++)
	{
		IndexCatalogBlock* block = (IndexCatalogBlock*)bufferManager->getBlock(IndexCatalogBlock::NAME, i);
		for(int j=0; j<block->getIndexCount(); j++)
		{
			QStringList info;
			info << block->getIndexName(j)
				 << block->getTableName(j)
				 << block->getAttrName(j);
			ret << info;
		}
	}

	return ret;
}
