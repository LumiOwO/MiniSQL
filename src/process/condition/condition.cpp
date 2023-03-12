#include "condition.h"
#include "../managers/managers.h"
#include "../blocks/blocks.h"
#include "../myerror/myerror.h"
#include "../BPlusTree/BPlusTree.h"

bool Condition::meets(const QStringList &attrNames, const QVector<Attribute::DataType> &dataTypes,
					  const QStringList &record, const QVector<Condition *> &conditions)
{
	bool ret = true;
	QVectorIterator<Condition*> iter(conditions);
	while(ret && iter.hasNext())
		ret = iter.next()->meets(attrNames, dataTypes, record);
	return ret;
}

bool Condition::meets(const QStringList &attrNames, const QVector<Attribute::DataType> &dataTypes,
					  const QStringList &record) const
{
	int index = -1;
	for(int i=0; i<attrNames.size(); i++)
		if( this->getAttrName() == attrNames[i] ||
			this->getAttrName() == this->getTableName() + "." + attrNames[i]) {
			index = i;
			break;
		}
	Attribute::DataType type = dataTypes[index];

	// get values
	QString leftValue = record[index].toLower();
	QString rightValue;
	if(!this->isRValueAttr())
	{
		// check type validity
		bool ok;
		if(type == Attribute::FLOAT)
			this->value.toDouble(&ok);
		else if(type == Attribute::INT)
			this->value.toInt(&ok);
		else
			ok = this->value.size() <= type;
		if(!ok)
			throw MyError::COND_TYPEERROR;

		rightValue = this->value.toLower();
	}
	else
		rightValue = record[attrNames.indexOf(this->value)].toLower();

	// compare
	bool ret;
	if(type == Attribute::FLOAT)
		ret = CompareExec<double>(leftValue.toDouble(), rightValue.toDouble());
	else if(type == Attribute::INT)
		ret = CompareExec<int>(leftValue.toInt(), rightValue.toInt());
	else
		ret = CompareExec<QString>(leftValue, rightValue);

//	qDebug() << attrName << compare << value;
//	qDebug() << leftValue << compare << rightValue;
//	qDebug() << ret;

	return ret;
}

QSet<QPair<int, int>>* Condition::getSatisfied() const
{
	QSet<QPair<int, int>>* ret;
	IndexManager *indexManager = IndexManager::getInstance();
	if(indexManager->hasIndex(this->tableName, this->attrName))
	{
		QString indexName = indexManager->getIndexName(this->tableName, this->attrName);
		BPlusTree tree(indexManager->getRoot(indexName));
		ret = tree.getSatisfiedSet(this);
	}
	else
	{
		ret = new QSet<QPair<int, int>>();
		BufferManager *bufferManager = BufferManager::getInstance();

		// get attribute parameters
		TableBlock* block = (TableBlock*)bufferManager->getBlock(this->tableName, 0);
		QVector<Attribute*> attributes = block->getAttributes();
		QStringList attrNames;
		QVector<Attribute::DataType> dataTypes;

		QVectorIterator<Attribute*> iter(attributes);
		while(iter.hasNext())
		{
			Attribute* attr = iter.next();
			attrNames << attr->getName();
			dataTypes << attr->getDataType();
			delete attr;
		}

		int blocks = bufferManager->getBlockNums(this->tableName);
		for(int i=0; i<blocks; i++)
		{
			block = (TableBlock*)bufferManager->getBlock(this->tableName, i);
			QVector<QStringList> records = block->getRecords();
			for(int j=0; j<records.size(); j++)
			{
				if(this->meets(attrNames, dataTypes, records[j]))
				{
					ret->insert(QPair<int, int>(i, j));
				}
			}
		}
	}
	return ret;
}
