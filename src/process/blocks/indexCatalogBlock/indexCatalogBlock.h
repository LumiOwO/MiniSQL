#ifndef _INDEXCATALOGBLOCK_H_
#define _INDEXCATALOGBLOCK_H_

#include "../block.h"

class IndexCatalogBlock: public Block
{
public:
	class Header
	{
	public:
		// data reference
		intRef count;

		Header(pToData dataAddr);
		uint bytes() const { return sizeof(count); }
	};
public:
	static const char ID = 'i';
	static constexpr const CString NAME = "indexcatalog";
private:
	IndexCatalogBlock::Header header;
	pToData indexZone;
public:
	IndexCatalogBlock(int No);
	IndexCatalogBlock(pToData data);

	// getter
	int getIndexCount() const { return header.count; }

	// insert index
	void insertIndex(QString indexName, QString tableName, QString attrName);
	// drop index
	void dropIndex(QString indexName);
	// get the name of index by attribute
	QString getIndexName(QString tableName, QString attrName);
	// get the name of index at i-th in this block
	QString getIndexName(int i) const { return indexZone + i * STRLEN_MAX * 3; }
	// get the name of table at i-th index in this block
	QString getTableName(int i) const { return indexZone + i * STRLEN_MAX * 3 + STRLEN_MAX; }
	// get the name of attr at i-th index in this block
	QString getAttrName(int i) const { return indexZone + i * STRLEN_MAX * 3 + STRLEN_MAX * 2; }
	// check whether this block can insert an index
	bool canInsert() const { return BLOCK_SIZE - offsetToEnd() >= STRLEN_MAX * 3; }
	// check whether the index is in the block
	bool hasIndex(QString indexName) const;
	// check whether the index of an attribute is in the block
	bool hasIndex(QString tableName, QString attrName) const;

	// debug
	friend QDebug operator<<(QDebug debug, const IndexCatalogBlock& block);
	friend QDebug operator<<(QDebug debug, const IndexCatalogBlock* block);
};

#endif
