#ifndef _LEAFBLOCK_H_
#define _LEAFBLOCK_H_

#include "../nodeBlock/nodeBlock.h"

class LeafBlock: public NodeBlock
{
public:
	class Header
	{
	public:
		intRef parentNo;
		uintRef unitBytes;
		intRef count;
		intRef dataType;
		intRef prevNo;
		intRef nextNo;

		Header(pToData dataAddr);
		uint bytes() const {
			return sizeof(parentNo) + sizeof(unitBytes) + sizeof(count)
					+ sizeof(dataType) + sizeof(prevNo) + sizeof(nextNo);
		}
	};
public:
	static const char ID = 'l';
private:
	LeafBlock::Header header;
	pToData valueZone;
public:
	LeafBlock(QString name, int No, Attribute::DataType type);
	LeafBlock(pToData data);

	// getters
	int getParentNo() const { return header.parentNo; }
	uint getUnitBytes() const { return header.unitBytes; }
	int getUnitCount() const { return header.count; }
	Attribute::DataType getDataType() const { return header.dataType; }
	int getMaxCount() const { return (BLOCK_SIZE - (valueZone - dataZone)) / int(getUnitBytes()); }
	int getPrevNo() const { return header.prevNo; }
	int getNextNo() const { return header.nextNo; }

	QString getFirstKey() const;
	QString getValueAt(int i) const;
	int getSerial(QString value) const;

	// setters
	void setParentNo(int No) { header.parentNo = No; }
	void setPrevNo(int No) { header.prevNo = No; }
	void setNextNo(int No) { header.nextNo = No; }

	// judgements
	bool canInsert() const { return BLOCK_SIZE - offsetToEnd() >= getUnitBytes(); }
	bool hasParent() const { return header.parentNo >= 0; }
	bool hasNext() const { return getNextNo() >= 0; }
	bool hasPrev() const { return getPrevNo() >= 0; }

	// insert
	virtual void insert(QVector<NodeBlock *>& nodes, QString value, int BlockNo, int serial);
	// delete
	virtual void deLete(QVector<NodeBlock *>& nodes, int serial);

	// push satisfied data into sets
	void pushAll(QSet<QPair<int, int>>* set);
	void pushEqual(QSet<QPair<int, int>>* set, QString value);
	void pushNotEqual(QSet<QPair<int, int>>* set, QString value);
	void pushLessThan(QSet<QPair<int, int>>* set, QString value);
	void pushLargerThan(QSet<QPair<int, int>>* set, QString value);
	void pushNotLessThan(QSet<QPair<int, int>>* set, QString value);
	void pushNotLargerThan(QSet<QPair<int, int>>* set, QString value);

	uint getValueBytes() const;

	// debug
	friend QDebug operator<<(QDebug debug, const LeafBlock& block);
	friend QDebug operator<<(QDebug debug, const LeafBlock* block);
private:
	LeafBlock(int No):
		LeafBlock("", No, 0) {}

	// compare
	bool lessThan(QString a, QString b) const;
	bool largerThan(QString a, QString b) const { return !lessThan(a, b) && a != b; }
	bool notLessThan(QString a, QString b) const { return !lessThan(a, b); }
	bool notLargerThan(QString a, QString b) const { return !largerThan(a, b); }
};

#endif
