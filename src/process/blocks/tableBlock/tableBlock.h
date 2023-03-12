#ifndef _TABLEBLOCK_H_
#define _TABLEBLOCK_H_

#include "../block.h"
#include "../../attribute/attribute.h"

class TableBlock: public Block
{
public:
	class Header
	{
	public:
		// data reference
		intRef count;
		intRef attrNum;
		uintRef recordBytes;

		Header(pToData startAddr);
		uint bytes() const { return sizeof(count) + sizeof(attrNum) + sizeof(recordBytes); }
	};
public:
	static const char ID = 't';
private:
	// header
	TableBlock::Header header;
	// data zone
	pToData attrZone;
	pToData recordZone;

public:
	TableBlock(QString name, QVector<Attribute*> attributes, int No);
	TableBlock(pToData data);

	// getter
	QString getTableName() const { return getName(); }
	int getRecordCount() const { return header.count; }
	int getMaxCount() const { return (BLOCK_SIZE - (recordZone - dataZone)) / int(getRecordBytes()); }
	int getAttrNum() const { return header.attrNum; }
	uint getRecordBytes() const { return header.recordBytes; }
	QVector<Attribute*> getAttributes() const;
	QVector<QStringList> getRecords() const;

	// get the No of an attribute
	int getAttrNo(QString attrName) const;
	// get the name of i-th attribute
	QString getAttrName(int i) const;
	// get the dataType of i-th attribute
	Attribute::DataType getDataType(int i) const;
	// get the uniqueness of i-th attribute
	bool isUniqueAt(int i) const;
	// check whether this block can insert a record
	bool canInsert(QStringList values) const;
	// insert record values into the block
	void insertRecord(QStringList values);
	void insertRecords(QVector<QStringList> values) {
		for(int i=0; i<values.size(); i++) insertRecord(values[i]);
	}
	// drop records by No
	void dropRecords(QVector<int> recordsNo);
	// remove all records
	void dropAllRecords();

	// debug
	friend QDebug operator<<(QDebug debug, const TableBlock& block);
	friend QDebug operator<<(QDebug debug, const TableBlock* block);
private:
	TableBlock(int No, QString name = "");
};

#endif
