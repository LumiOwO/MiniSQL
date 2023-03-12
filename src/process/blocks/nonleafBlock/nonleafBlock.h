#ifndef _NONLEAFBLOCK_H_
#define _NONLEAFBLOCK_H_

#include "../nodeBlock/nodeBlock.h"

class NonLeafBlock: public NodeBlock
{
public:
	class Header
	{
	public:
		intRef parentNo;
		uintRef unitBytes;
		intRef count;
		intRef dataType;
		intRef firstChildNo;

		Header(pToData dataAddr);
		uint bytes() const {
			return sizeof(parentNo) + sizeof(unitBytes) + sizeof(count)
					+ sizeof(dataType) + sizeof(firstChildNo);
		}
	};
public:
	static const char ID = 'n';
private:
	NonLeafBlock::Header header;
	pToData keyZone;
public:
	NonLeafBlock(QString name, int No, Attribute::DataType type, int firstChildNo);
	NonLeafBlock(pToData data);

	// getters
	int getParentNo() const { return header.parentNo; }
	uint getUnitBytes() const { return header.unitBytes; }
	int getUnitCount() const { return header.count; }
	Attribute::DataType getDataType() const { return header.dataType; }
	int getMaxCount() const { return (BLOCK_SIZE - (keyZone - dataZone)) / int(getUnitBytes()); }
	int getFirstChildNo() const { return header.firstChildNo; }
	// setters
	void setParentNo(int No) { header.parentNo = No; }

	QString getFirstKey() const;
	int getChildNo(QString value) const;

	bool canInsert() const { return BLOCK_SIZE - offsetToEnd() >= getUnitBytes(); }
	bool hasParent() const { return header.parentNo >= 0; }

	virtual void insert(QVector<NodeBlock *>& nodes, QString value, int BlockNo, int serial = 0);
	virtual void deLete(QVector<NodeBlock *>& nodes, int serial);

	// debug
	friend QDebug operator<<(QDebug debug, const NonLeafBlock& block);
	friend QDebug operator<<(QDebug debug, const NonLeafBlock* block);

private:
	NonLeafBlock(int No):
		NonLeafBlock("", No, 0, 0) {}
};
#endif
