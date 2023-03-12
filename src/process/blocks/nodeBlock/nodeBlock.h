#ifndef _NODEBLOCK_H_
#define _NODEBLOCK_H_

#include "../block.h"
#include "../../attribute/attribute.h"

// interface
class NodeBlock: public Block
{
public:
	NodeBlock(char id, int No, QString name);

	// getters
	virtual int getParentNo() const = 0;
	virtual uint getUnitBytes() const = 0;
	virtual int getUnitCount() const = 0;
	virtual Attribute::DataType getDataType() const = 0;
	virtual QString getFirstKey() const = 0;
	virtual int getMaxCount() const = 0;

	// setters
	virtual void setParentNo(int No) = 0;

	// judgement
	virtual bool canInsert() const = 0;
	virtual bool hasParent() const = 0;

	virtual void insert(QVector<NodeBlock *>& nodes, QString value, int BlockNo, int serial = -1) = 0;
	virtual void deLete(QVector<NodeBlock *>& nodes, int serial) = 0;

	// get block
	static NodeBlock* getBlock(const QVector<NodeBlock *>& nodes, int BlockID);

	// debug
	friend QDebug operator<<(QDebug debug, const NodeBlock& block);
	friend QDebug operator<<(QDebug debug, const NodeBlock* block);
};


#endif
