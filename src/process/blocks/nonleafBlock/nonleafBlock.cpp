#include "../blocks.h"

NonLeafBlock::Header::Header(pToData dataAddr):
	parentNo(*(int*)(dataAddr)),
	unitBytes(*(uint*)(dataAddr + sizeof(parentNo))),
	count(*(int*)(dataAddr + sizeof(parentNo) + sizeof(unitBytes))),
	dataType(*(int*)(dataAddr + sizeof(parentNo) + sizeof(unitBytes) + sizeof(count))),
	firstChildNo(*(int*)(dataAddr + sizeof(parentNo) + sizeof(unitBytes) + sizeof(count) + sizeof(dataType)))
{
	count = 0;
	parentNo = -1;
}

NonLeafBlock::NonLeafBlock(QString name, int No, Attribute::DataType type, int firstChildNo):
	NodeBlock(ID, No, name),
	header((pToData)dataZone + offsetToBegin()),
	keyZone((pToData)dataZone + offsetToBegin() + header.bytes())
{
	header.firstChildNo = firstChildNo;
	modifyValidBytes(+int(header.bytes()));

	// compute unit bytes
	header.dataType = type;
	if(type == Attribute::INT)
		header.unitBytes = sizeof(int);
	else if(type == Attribute::FLOAT)
		header.unitBytes = sizeof(double);
	else
		header.unitBytes = uint(type) + 1;
	header.unitBytes += sizeof(int);
}

NonLeafBlock::NonLeafBlock(pToData data):
	NonLeafBlock(0)
{
	memcpy((pToData)dataZone, data, BLOCK_SIZE);
}

QString NonLeafBlock::getFirstKey() const
{
	pToData p = keyZone;
	QString ret;
	Attribute::DataType type = getDataType();
	if(type == Attribute::INT) {
		int key;
		memcpy(&key, p, sizeof(int));
		ret = QString::number(key);
	} else if(type == Attribute::FLOAT) {
		double key;
		memcpy(&key, p, sizeof(double));
		ret = QString::number(key);
	} else {
		ret = p;
	}
	return ret;
}

int NonLeafBlock::getChildNo(QString value) const
{
	pToData p = keyZone;
	int ret = -1;
	Attribute::DataType type = getDataType();
	// compare first
	if(type == Attribute::INT) {
		int key;
		memcpy(&key, p, sizeof(int));
		if(key > value.toInt()) {
			ret = this->getFirstChildNo();
		}
	} else if(type == Attribute::FLOAT) {
		double key;
		memcpy(&key, p, sizeof(double));
		if(key > value.toDouble()) {
			ret = this->getFirstChildNo();
		}
	} else {
		if(QString(p) > value) {
			ret = this->getFirstChildNo();
		}
	}

	// compare others
	for(int i=0; ret < 0 && i<getUnitCount()-1; i++) {
		if(type == Attribute::INT) {
			int key;
			memcpy(&key, p + getUnitBytes(), sizeof(int));
			if(key > value.toInt()) {
				ret = *(int*)(p + sizeof(int));
			}
		} else if(type == Attribute::FLOAT) {
			double key;
			memcpy(&key, p + getUnitBytes(), sizeof(double));
			if(key > value.toDouble()) {
				ret = *(int*)(p + sizeof(double));
			}
		} else {
			if(QString(p + getUnitBytes()) > value) {
				ret = *(int*)(p + type+1);
			}
		}
		p += getUnitBytes();
	}

	if(ret < 0) {
		if(type == Attribute::INT)
			ret = *(int*)(p + sizeof(int));
		else if(type == Attribute::FLOAT)
			ret = *(int*)(p + sizeof(double));
		else
			ret = *(int*)(p + type+1);
	}
	return ret;
}

void NonLeafBlock::insert(QVector<NodeBlock *>& nodes, QString value, int BlockNo, int)
{
	setDirty(true);
	Attribute::DataType type = getDataType();
	if(canInsert())
	{
		// find the position to insert
		pToData p = keyZone;
		for(int i=0; i<getUnitCount(); i++)
		{
			if(type == Attribute::INT) {
				int key;
				memcpy(&key, p, sizeof(int));
				if(key > value.toInt()) break;
			} else if(type == Attribute::FLOAT) {
				double key;
				memcpy(&key, p, sizeof(double));
				if(key > value.toDouble()) break;
			} else {
				QString key = p;
				if(key > value) break;
			}
			p += getUnitBytes();
		}
		// move the data to get empty space
		pToData q = p + getUnitBytes();
		memmove(q, p, uint(dataZone + offsetToEnd() - p));

		// write data
		if(type == Attribute::INT) {
			int key = value.toInt();
			memcpy(p, &key, sizeof(int));
			p += sizeof(int);
		} else if(type == Attribute::FLOAT) {
			double key = value.toDouble();
			memcpy(p, &key, sizeof(double));
			p += sizeof(double);
		} else {
			memcpy(p, value.toStdString().data(), uint(type) + 1);
			p += type + 1;
		}
		memcpy(p, &BlockNo, sizeof(int));
		p += sizeof(int);

		// modify info
		header.count ++;
		modifyValidBytes(+int(getUnitBytes()));
	}
	else
	{
		// split values to the two blocks
		int remainNum = getUnitCount() / 2;
		int movedNum = getUnitCount() - remainNum;
		pToData p = this->keyZone + uint(remainNum) * getUnitBytes() - getUnitBytes();
		int newFirstChildNo;
		if(type == Attribute::INT) {
			newFirstChildNo = *(int*)(p + sizeof(int));
		} else if(type == Attribute::FLOAT) {
			newFirstChildNo = *(int*)(p + sizeof(double));
		} else {
			newFirstChildNo = *(int*)(p + type + 1);
		}
		p += getUnitBytes();

		// get new block
		int newNo = nodes.last()->getNo() + 1;
		NonLeafBlock* newBlock = new NonLeafBlock(this->getName(), newNo, this->getDataType(), newFirstChildNo);
		// modify parents
		newBlock->setParentNo(this->getParentNo());
		// set new block data
		memcpy(newBlock->keyZone, p, uint(movedNum) * getUnitBytes());

		// modify new block info
		newBlock->modifyValidBytes(+movedNum * int(getUnitBytes()));
		newBlock->header.count = movedNum;
		// modify the info this block
		this->modifyValidBytes(-movedNum * int(getUnitBytes()));
		header.count = remainNum;
		// push the new block into the nodes
		nodes.push_back(newBlock);

		// insert the new value
		if(value < newBlock->getFirstKey())
			this->insert(nodes, value, BlockNo);
		else
			newBlock->insert(nodes, value, BlockNo);

		// insert parent
		NonLeafBlock *parent;
		if(!this->hasParent())
		{
			parent = new NonLeafBlock(this->getName(), newNo+1, this->getDataType(), this->getNo());
			this->setParentNo(parent->getNo());
			newBlock->setParentNo(parent->getNo());
			nodes.push_back(parent);
		}
		else
			parent = (NonLeafBlock*)getBlock(nodes, this->getParentNo());

		parent->insert(nodes, newBlock->getFirstKey(), newBlock->getNo());
	}
}

void NonLeafBlock::deLete(QVector<NodeBlock *>& nodes, int serial)
{
	setDirty(true);
	// delete one key
	Attribute::DataType type = getDataType();
	// find the position to delete
	pToData p = keyZone + uint(serial) * getUnitBytes();
	pToData q = p + getUnitBytes();
	memcpy(p, q, uint(dataZone + offsetToEnd() - q));

	// modify info
	header.count --;
	modifyValidBytes(-int(getUnitBytes()));

	if(this->hasParent() && getUnitCount() < getMaxCount() / 2)
	{
		int sum = this->getUnitCount();
		if(sum > getMaxCount())
		{
			int leftNum = sum / 2;

			pToData p = keyZone + leftNum * getUnitCount();
			for(int i=leftNum; i<getUnitCount(); i++)
			{
				QString value;
				if(type == Attribute::INT) {
					value = QString::number(*(int*)p);
					p += sizeof(int);
				} else if(type == Attribute::FLOAT) {
					value = QString::number(*(double*)p);
					p += sizeof(double);
				} else {
					value = p;
					p += type + 1;
				}
				int BlockNo = *(int*)p;
				p += sizeof(int);
				int serial = *(int*)p;
				p += sizeof(int);

				insert(nodes, value, BlockNo, serial);
			}
		}
	}
}

QDebug operator<<(QDebug debug, const NonLeafBlock& block)
{
	debug << "block name: " << block.getName() << endl;
	debug << "block id: " << block.getID() << endl;
	debug << "block No: " << block.getNo() << endl;
	debug << "isDirty: " << block.isdirty() << endl;
	debug << "isPinned: " << block.isPinned() << endl;
	debug << "valid Bytes: " << block.offsetToEnd() << endl;

	debug << "parentNo:" << block.getParentNo() << endl;
	debug << "unitBytes:" << block.getUnitBytes() << endl;
	debug << "count:" << block.getUnitCount() << endl;
	debug << "dataType:" << block.getDataType() << endl;

	debug << "firstKey" << block.getFirstKey() << endl;

	Byte *p = block.keyZone;
	Attribute::DataType type = block.getDataType();
	for(int i=0; i<block.getUnitCount(); i++)
	{
		debug << i << ":";
		if(type == Attribute::INT) {
			debug << *(int*)p;
			p += sizeof(int);
		} else if(type == Attribute::FLOAT) {
			debug << *(double*)p;
			p += sizeof(double);
		} else {
			debug << p;
			p += type + 1;
		}
		debug << "ChildNo:" << *(int*)p << endl;
		p += sizeof(int);
	}

	return debug;
}

QDebug operator<<(QDebug debug, const NonLeafBlock* block)
{
	return block? (debug << *block): debug << "NULL";
}
