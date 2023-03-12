#include "../managers.h"
#include "../../blocks/blocks.h"
#include "../../myerror/myerror.h"
#include "../../BPlusTree/BPlusTree.h"

RecordManager *RecordManager::instance = nullptr;

RecordManager *RecordManager::getInstance()
{
	if(!instance)
		instance = new RecordManager();
	return instance;
}

void RecordManager::free()
{
	if(instance)
	{
		delete instance;
		instance = nullptr;
	}
}

QVector<QPair<QString, QPair<int, int>>> RecordManager::getAllValues(QString tableName, QString attrName) const
{
	QVector<QPair<QString, QPair<int, int>>> ret;
	BufferManager *bufferManager = BufferManager::getInstance();
	int num = bufferManager->getBlockNums(tableName);
	for(int i=0; i<num; i++)
	{
		TableBlock* block = (TableBlock*)bufferManager->getBlock(tableName, i);
		QVector<QStringList> records = block->getRecords();
		int attrNo = block->getAttrNo(attrName);
		for(int j=0; j<records.size(); j++)
		{
			QString value = records[j][attrNo];
			int BlockNo = i;
			int serial = j;

			ret << QPair<QString, QPair<int, int>>(value, QPair<int, int>(BlockNo, serial));
		}
	}

	return ret;
}

void RecordManager::insert(QString tableName, QStringList values)
{
	// get managers
	BufferManager *bufferManager = BufferManager::getInstance();
	CatalogManager *catalogManager = CatalogManager::getInstance();
	IndexManager *indexManager = IndexManager::getInstance();

	if(!catalogManager->hasTable(tableName))
		throw MyError::TABLE_NOTFOUND;

	// insert
	int last = bufferManager->getBlockNums(tableName) - 1;
	TableBlock* table = (TableBlock*)bufferManager->getBlock(tableName, last);
	if(!table->canInsert(values)) {
		table = new TableBlock(tableName, table->getAttributes(), ++last);
		table->setDirty(true);
		bufferManager->EnqueueBlock(table);
	}

	table->insertRecord(values);

	// modify index
	for(int i=0; i<table->getAttrNum(); i++)
		if(table->isUniqueAt(i) && indexManager->hasIndex(tableName, table->getAttrName(i)))
		{
			QString indexName = indexManager->getIndexName(tableName, table->getAttrName(i));
			NodeBlock* root = indexManager->getRoot(indexName);
			BPlusTree tree(root);
			QVector<QPair<QString, QPair<int, int>>> insert = {
				QPair<QString, QPair<int, int>>(
					values[i], QPair<int, int>(table->getNo(), table->getRecordCount()-1)
				)
			};
			tree.insert(insert);
		}
//	qDebug() << table;
}

void RecordManager::select(QStringList select, QStringList from, QVector<Condition*> conditions)
{
	// 检查表名正确
	// 把表的属性串起来
	// 检查属性名正确-->条件 左值右值

	// 判断条件
	// 对每个比较值的条件，返回所有符合的(块号, 序号)
	// 根据pair创建表

	// join-->大表
	// 取子表

	// check table name
	checkName(from);
	// check attributes and conditions
	checkAttr(select, from, conditions);

	// get index of records whose value meets the conditions
	QVector<QSet<QPair<int, int>>* > recordList(from.size());
	int size = conditions.size();
	for(int i=0; i<size; i++)
	{
		Condition* condition = conditions.takeFirst();
		if(condition->isRValueAttr())
			conditions.push_back(condition);
		else
		{
			int index = from.indexOf(condition->getTableName().toLower());
			QSet<QPair<int, int>> *set = condition->getSatisfied();
			if(!recordList[index])
				recordList[index] = set;
			else {
				recordList[index]->intersect(*set);
				delete set;
			}
		}
	}
	// join all tables
	Table table;
	bool withTableName = from.size() > 1;
	for(int i=0; i<from.size(); i++)
	{
		table.join(from[i], recordList[i], withTableName);
		delete recordList[i];
	}
	// get sub table
	setViewTable(table.subTable(select, conditions));

//	qDebug() << this->viewTable;
}

void RecordManager::deLete(QString from, QVector<Condition *> conditions)
{
	BufferManager *bufferManager = BufferManager::getInstance();
	IndexManager *indexManager = IndexManager::getInstance();

	// check table name
	checkName(from);
	// check attributes and conditions
	checkAttr(from, conditions);

	// get index of records whose value meets the conditions
	QSet<QPair<int, int>>* recordList = nullptr;
	int size = conditions.size();
	for(int i=0; i<size; i++)
	{
		Condition* condition = conditions.takeFirst();
		if(condition->isRValueAttr())
			conditions.push_back(condition);
		else
		{
			QSet<QPair<int, int>> *set = condition->getSatisfied();
			if(!recordList)
				recordList = set;
			else {
				recordList->intersect(*set);
				delete set;
			}
		}
	}

	// modify index
	TableBlock *block = nullptr;
	if(!recordList)
	{
		block = (TableBlock*)bufferManager->getBlock(from, 0);
		for(int i=0; i<block->getAttrNum(); i++)
		{
			QString attrName = block->getAttrName(i);
			if(indexManager->hasIndex(from, attrName))
			{
				QString indexName = indexManager->getIndexName(from, attrName);
				NodeBlock* root = indexManager->getRoot(indexName);
				BPlusTree tree(root);
				tree.deLeteAll();
			}

		}
	}
	else
	{
		QVector<int> hasIndexSerials;
		block = (TableBlock*)bufferManager->getBlock(from, 0);
		for(int i=0; i<block->getAttrNum(); i++)
			if(indexManager->hasIndex(from, block->getAttrName(i)))
				hasIndexSerials << i;

		QVector<QPair<QString, QStringList>> indexInfo;
		QSetIterator<QPair<int, int>> iter(*recordList);
		while(iter.hasNext())
		{
			QPair<int, int> pair = iter.next();
			int BlockNo = pair.first;
			int serial = pair.second;

			block = (TableBlock*)bufferManager->getBlock(from, BlockNo);
			QStringList record = block->getRecords()[serial];
			for(int i=0; i<hasIndexSerials.size(); i++)
			{
				int attrNo = hasIndexSerials[i];
				QString value = record[attrNo];
				QString indexName = indexManager->getIndexName(from, block->getAttrName(attrNo));
				int j;
				for(j=0; j<indexInfo.size(); j++)
					if(indexInfo[j].first == indexName)
					{
						indexInfo[j].second << value;
						break;
					}
				if(j == indexInfo.size())
					indexInfo << QPair<QString, QStringList>(indexName, QStringList({value}));
			}
		}
		for(int i=0; i<indexInfo.size(); i++)
		{
			QString indexName = indexInfo[i].first;
			QStringList values = indexInfo[i].second;
			NodeBlock* root = indexManager->getRoot(indexName);
			BPlusTree tree(root);
			tree.deLete(values);
		}
	}

	// delete data
	int num = bufferManager->getBlockNums(from);
	for(int i=0; i<num; i++)
	{
		block = (TableBlock*)bufferManager->getBlock(from, i);
		int cnt = block->getRecordCount();
		QVector<int> dropNo;
		for(int j=0; j<cnt; j++)
			if(!recordList || recordList->contains(QPair<int, int>(i, j)))
				dropNo << j;
		block->dropRecords(dropNo);
	}
	delete recordList;
	// compact all the blocks to make sure only the last block has empty space
	compact(from);

}

void RecordManager::checkName(const QStringList &from)
{
	CatalogManager *catalogManager = CatalogManager::getInstance();
	BufferManager *bufferManager = BufferManager::getInstance();

	// check table name
	QSet<QString> checkSet;
	QStringListIterator iter(from);
	while(iter.hasNext())
	{
		QString tableName = iter.next().toLower();
		// check exist table
		if(!catalogManager->hasTable(tableName))
			throw MyError::TABLE_NOTFOUND;
		// check duplicated table names
		if(!checkSet.contains(tableName))
			checkSet.insert(tableName);
		else
			throw MyError::JOIN_ITSELT;
		// check empty table
		TableBlock* block = (TableBlock*)bufferManager->getBlock(tableName, 0);
		if(block->getRecordCount() == 0)
			throw MyError::JOIN_ERROR;
	}
}

void RecordManager::checkAttr(QStringList& select, QStringList& from, QVector<Condition*>& conditions)
{
	CatalogManager *catalogManager = CatalogManager::getInstance();

	// create attribute list
	QVector<QStringList> attrList;
	for(int i=0; i<from.size(); i++)
		attrList << catalogManager->getAttrNames(from[i].toLower());

	// check condition value
	for(int i=0; i<conditions.size(); i++)
	{
		QString leftValue = conditions[i]->getAttrName();
		QString rightValue = conditions[i]->getValue();
		// check left value
		QStringList tables = inWhichTable(leftValue, attrList, from);
		if(tables.size() == 0)
			throw MyError::ATTR_NOTFOUND_COND;
		else if(tables.size() > 1)
			throw MyError::ATTR_AMBIGUOUS_COND;
		else
			conditions[i]->setTableName(tables[0]);
		// check right value
		int occurCnt = inWhichTable(rightValue, attrList, from).size();
		if(occurCnt > 1)
			throw MyError::ATTR_AMBIGUOUS_COND;
		else if(occurCnt == 1)
			conditions[i]->setRValueAttr(true);
	}

	// check select attributes
	QStringListIterator iter(select);
	while(iter.hasNext())
	{
		QString attrName = iter.next().toLower();
		// if select *, then get all attributes
		if(attrName == "*")
		{
			select.clear();
			for(int i=0; i<attrList.size(); i++)
				for(int j=0; j<attrList[i].size(); j++)
					select << (from.size() > 1? from[i].toLower() + ".": "") + attrList[i][j];
			break;
		}
		// find the attribute in the list
		int occurCnt = inWhichTable(attrName, attrList, from).size();
		if(occurCnt == 0)
			throw MyError::ATTR_NOTFOUND;
		else if(occurCnt > 1)
			throw MyError::ATTR_AMBIGUOUS;
	}
}

QStringList RecordManager::inWhichTable(const QString &attr, const QVector<QStringList> &attrList, QStringList from)
{
	QStringList tables;
	for(int i=0; i<attrList.size(); i++) {
		QStringList attrs = attrList[i];
		for(int j=0; j<attrs.size(); j++) {
			if(attr == attrs[j] || attr == from[i].toLower() + "." + attrs[j]) {
				tables << from[i].toLower();
				break;
			}
		}
	}
	return tables;
}

void RecordManager::compact(QString tableName)
{
	BufferManager *bufferManager = BufferManager::getInstance();
	int num = bufferManager->getBlockNums(tableName);
	int cnt = 0;

	QVector<QStringList> records;
	TableBlock* block = nullptr;
	for(int i=0; i<num; i++)
	{
		block = (TableBlock*)bufferManager->getBlock(tableName, i);
		int maxCnt = block->getMaxCount();
		QVector<QStringList> list = block->getRecords();
		block->dropAllRecords();

		for(int j=0; j<list.size(); j++) {
			records << list[j];
			if(records.size() == maxCnt) {
				block = (TableBlock*)bufferManager->getBlock(tableName, cnt++);
				block->insertRecords(records);
				records.clear();
			}
		}
	}

	for(int i=num-1; i>=cnt; i--) {
		block = (TableBlock*)bufferManager->getBlock(tableName, i);
		if(i == cnt && !records.isEmpty()) {
			block->insertRecords(records);
			records.clear();
		} else if (i != 0) {
			bufferManager->removeBlock(block);
		}
	}
}
