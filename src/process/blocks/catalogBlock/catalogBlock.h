#ifndef _CATALOGBLOCK_H_
#define _CATALOGBLOCK_H_

#include "../block.h"

class CatalogBlock: public Block
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
	static const char ID = 'c';
	static constexpr const CString NAME = "catalog";
private:
	// header
	CatalogBlock::Header header;
	// data zone
	pToData tableZone;
public:
	CatalogBlock(int No);
	CatalogBlock(pToData data);

	// getter
	int getTableCount() const { return header.count; }

	// get the name of table at i-th in this block
	QString getTableName(int i) const { return tableZone + i * STRLEN_MAX; }
	// insert table
	void insertTable(QString name);
	// drop table
	void dropTable(QString name);
	// check whether this block can insert a table name
	bool canInsert() const { return BLOCK_SIZE - offsetToEnd() >= STRLEN_MAX; }
	// check whether the table is in the block
	bool hasTable(QString name) const;

	// debug
	friend QDebug operator<<(QDebug debug, const CatalogBlock& block);
	friend QDebug operator<<(QDebug debug, const CatalogBlock* block);
};

#endif
