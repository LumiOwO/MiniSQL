#include "table.h"
#include "../managers/managers.h"

void Table::join(QString tableName, QSet<QPair<int, int>>* recordsIdx, bool withTableName)
{
	BufferManager *manager = BufferManager::getInstance();
	int num = manager->getBlockNums(tableName);

	// join attribute names and dataType
	TableBlock *block = (TableBlock*)manager->getBlock(tableName, 0);
	QVector<Attribute*> attributes = block->getAttributes();
	int attrNum = attributes.size();

	for(int i=0; i<attrNum; i++)
	{
		Attribute* attribute = attributes[i];
		QString attrName = attribute->getName();

		this->attrNames << (withTableName? tableName + ".": "") + attrName;
		this->dataTypes << attribute->getDataType();

		delete attribute;
	}

	// get records
	QVector<QStringList> outerRecords;
	for(int i=0; i<num; i++)
	{
		block = (TableBlock*)manager->getBlock(tableName, i);
		QVector<QStringList> records = block->getRecords();
		for(int j=0; j<records.size(); j++)
			if(!recordsIdx || recordsIdx->contains(QPair<int, int>(i, j)))
				outerRecords.append(records[j]);
	}

	// join records
	QVector<QStringList> temp;
	if(this->records.size() == 0)
		temp = outerRecords;
	else {
		for(int i=0; i<outerRecords.size(); i++)
			temp.append(this->records);

		for(int i=0; i<outerRecords.size(); i++)
			for(int j=0; j<this->records.size(); j++)
				temp[i*this->records.size() + j] << outerRecords[i];
	}
	this->records = temp;
}

Table Table::subTable(QStringList select, QVector<Condition*> conditions) const
{
	Table table;
	// get serials of select attributes
	QVector<int> serials;
	for(int i=0; i<select.size(); i++)
	{
		int index = attrNames.indexOf(select[i].toLower());
		serials << index;
		table.attrNames << attrNames[index];
		table.dataTypes << dataTypes[index];
	}

	for(int i=0; i<records.size(); i++)
	{
		QStringList record = records[i];
		if(Condition::meets(attrNames, dataTypes, record, conditions))
		{
			QStringList in;
			for(int j=0; j<serials.size(); j++)
				in << record[serials[j]];
			table.records << in;
		}
	}
	return table;
}

void Table::copy(const Table &table)
{
	copy(&table);
}

void Table::copy(const Table *table)
{
	this->attrNames = table->attrNames;
	this->dataTypes = table->dataTypes;
	this->records = table->records;
}

QDebug operator<<(QDebug debug, const Table& table)
{
	debug << "attribute names:" << table.attrNames << endl;
	debug << "data types:" << table.dataTypes << endl;
	for(int i=0; i<table.records.size(); i++)
		debug << "record" << i << ":" << table.records[i] << endl;

	return debug;
}

QDebug operator<<(QDebug debug, const Table* table)
{
	return debug << *table;
}
