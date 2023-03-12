#include "../blocks.h"

LeafBlock::Header::Header(pToData dataAddr):
	parentNo(*(int*)(dataAddr)),
	unitBytes(*(uint*)(dataAddr + sizeof(parentNo))),
	count(*(int*)(dataAddr + sizeof(parentNo) + sizeof(unitBytes))),
	dataType(*(int*)(dataAddr + sizeof(parentNo) + sizeof(unitBytes) + sizeof(count))),
	prevNo(*(int*)(dataAddr + sizeof(parentNo) + sizeof(unitBytes) + sizeof(count) + sizeof(dataType))),
	nextNo(*(int*)(dataAddr + sizeof(parentNo) + sizeof(unitBytes) + sizeof(count) + sizeof(dataType) + sizeof(prevNo)))
{
	count = 0;
	parentNo = -1;
	prevNo = nextNo = -1;
}

LeafBlock::LeafBlock(QString name, int No, Attribute::DataType type):
	NodeBlock(ID, No, name),
	header((pToData)dataZone + offsetToBegin()),
	valueZone((pToData)dataZone + offsetToBegin() + header.bytes())
{
	modifyValidBytes(+int(header.bytes()));

	// compute unit bytes;
	header.dataType = type;
	if(type == Attribute::INT)
		header.unitBytes = sizeof(int);
	else if(type == Attribute::FLOAT)
		header.unitBytes = sizeof(double);
	else
		header.unitBytes = uint(type) + 1;
	header.unitBytes += 2*sizeof(int);
}

LeafBlock::LeafBlock(pToData data):
	LeafBlock(0)
{
	memcpy((pToData)dataZone, data, BLOCK_SIZE);
}

QString LeafBlock::getFirstKey() const
{
	return getValueAt(0);
}

QString LeafBlock::getValueAt(int i) const
{
	pToData p = valueZone + uint(i) * getUnitBytes();
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

int LeafBlock::getSerial(QString value) const
{
	pToData p = valueZone;
	Attribute::DataType type = getDataType();
	int ret = -1;
	for(int i=0; i<getUnitCount(); i++)
	{
		if(type == Attribute::INT) {
			int key;
			memcpy(&key, p, sizeof(int));
			if(key == value.toInt()) {
				ret = i;
				break;
			}
		} else if(type == Attribute::FLOAT) {
			double key;
			memcpy(&key, p, sizeof(double));
			if((key - value.toDouble()) < 1e-5) {
				ret = i;
				break;
			}
		} else {
			QString key = p;
			if(key == value) {
				ret = i;
				break;
			}
		}
		p += getUnitBytes();
	}
	return ret;
}

void LeafBlock::insert(QVector<NodeBlock *>& nodes, QString value, int BlockNo, int serial)
{
	setDirty(true);
	if(canInsert())
	{
		Attribute::DataType type = getDataType();
		// find the position to insert
		pToData p = valueZone;
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
		memcpy(p, &serial, sizeof(int));
		p += sizeof(int);

		// modify info
		header.count ++;
		modifyValidBytes(+int(getUnitBytes()));
	}
	else
	{
		// get new block
		int newNo = nodes.last()->getNo() + 1;
		LeafBlock* newBlock = new LeafBlock(this->getName(), newNo, this->getDataType());
		// modify linked list
		newBlock->setNextNo(this->getNextNo());
		newBlock->setPrevNo(this->getNo());
		if(this->hasNext())
		{
			LeafBlock* block = (LeafBlock*)getBlock(nodes, this->getNextNo());
			block->setPrevNo(newBlock->getNo());
		}
		this->setNextNo(newBlock->getNo());
		// modify parents
		newBlock->setParentNo(this->getParentNo());

		// split values to the two blocks
		int remainNum = getUnitCount() / 2;
		int movedNum = getUnitCount() - remainNum;

		// set new block data
		pToData p = this->valueZone + uint(remainNum) * getUnitBytes();
		memcpy(newBlock->valueZone, p, uint(movedNum) * getUnitBytes());

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
			this->insert(nodes, value, BlockNo, serial);
		else
			newBlock->insert(nodes, value, BlockNo, serial);

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

void LeafBlock::deLete(QVector<NodeBlock *>& nodes, int serial)
{
	setDirty(true);
	// delete one key
	Attribute::DataType type = getDataType();
	// find the position to delete
	pToData p = valueZone + uint(serial) * getUnitBytes();
	pToData q = p + getUnitBytes();
	memcpy(p, q, uint(dataZone + offsetToEnd() - q));

	// modify info
	header.count --;
	modifyValidBytes(-int(getUnitBytes()));

	if(this->hasParent() && getUnitCount() < getMaxCount() / 2)
	{
		LeafBlock* sibling;
		if(this->hasNext())
			sibling = (LeafBlock*)getBlock(nodes, this->getNextNo());
		else
			sibling = (LeafBlock*)getBlock(nodes, this->getPrevNo());

		LeafBlock *left;
		LeafBlock *right;
		if(this->getPrevNo() == sibling->getNo())
		{
			left = sibling;
			right = this;
		}
		else
		{
			left = this;
			right = sibling;
		}

		int sum = left->getUnitCount() + right->getUnitCount();
		if(sum > getMaxCount())
		{
			int leftNum = sum / 2;
			int rightNum = sum - leftNum;

			pToData p = left->valueZone + leftNum * getUnitCount();
			for(int i=leftNum; i<left->getUnitCount(); i++)
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

				right->insert(nodes, value, BlockNo, serial);
			}

			left->header.count = leftNum;
			left->modifyValidBytes(-rightNum * int(getUnitBytes()));
		}
	}
}

void LeafBlock::pushAll(QSet<QPair<int, int>>* set)
{
	pToData p = valueZone;
	for(int i=0; i<getUnitCount(); i++)
	{
		p += getValueBytes();
		int BlockNo = *(int*)p;
		p += sizeof(int);
		int serial = *(int*)p;
		p += sizeof(int);

		set->insert(QPair<int, int>(BlockNo, serial));
	}
}

void LeafBlock::pushEqual(QSet<QPair<int, int>>* set, QString value)
{
	pToData p = valueZone;
	for(int i=0; i<getUnitCount(); i++) {
		if (value == getValueAt(i) ) {
			pToData temp = p + getValueBytes();
			int BlockNo = *(int*)temp;
			temp += sizeof(int);
			int serial = *(int*)temp;
			temp += sizeof(int);

			set->insert(QPair<int, int>(BlockNo, serial));
			break;
		}
		p += getUnitBytes();
	}
}

void LeafBlock::pushNotEqual(QSet<QPair<int, int>>* set, QString value)
{
	pToData p = valueZone;
	for(int i=0; i<getUnitCount(); i++) {
		if (value != getValueAt(i) ) {
			pToData temp = p + getValueBytes();
			int BlockNo = *(int*)temp;
			temp += sizeof(int);
			int serial = *(int*)temp;
			temp += sizeof(int);
			set->insert(QPair<int, int>(BlockNo, serial));
		}
		p += getUnitBytes();
	}
}

void LeafBlock::pushLessThan(QSet<QPair<int, int>>* set, QString value)
{
	pToData p = valueZone;
	for(int i=0; i<getUnitCount(); i++) {
		if (lessThan(getValueAt(i), value) ) {
			pToData temp = p + getValueBytes();
			int BlockNo = *(int*)temp;
			temp += sizeof(int);
			int serial = *(int*)temp;
			temp += sizeof(int);
			set->insert(QPair<int, int>(BlockNo, serial));
		}
		p += getUnitBytes();
	}
}

void LeafBlock::pushLargerThan(QSet<QPair<int, int>>* set, QString value)
{
	pToData p = valueZone;
	for(int i=0; i<getUnitCount(); i++) {
		if (largerThan(getValueAt(i), value) ) {
			pToData temp = p + getValueBytes();
			int BlockNo = *(int*)temp;
			temp += sizeof(int);
			int serial = *(int*)temp;
			temp += sizeof(int);
			set->insert(QPair<int, int>(BlockNo, serial));
		}
		p += getUnitBytes();
	}
}

void LeafBlock::pushNotLessThan(QSet<QPair<int, int>>* set, QString value)
{
	pToData p = valueZone;
	for(int i=0; i<getUnitCount(); i++) {
		if (notLessThan(getValueAt(i), value) ) {
			pToData temp = p + getValueBytes();
			int BlockNo = *(int*)temp;
			temp += sizeof(int);
			int serial = *(int*)temp;
			temp += sizeof(int);
			set->insert(QPair<int, int>(BlockNo, serial));
		}
		p += getUnitBytes();
	}
}

void LeafBlock::pushNotLargerThan(QSet<QPair<int, int>>* set, QString value)
{
	pToData p = valueZone;
	for(int i=0; i<getUnitCount(); i++) {
		if (notLargerThan(getValueAt(i), value) ) {
			pToData temp = p + getValueBytes();
			int BlockNo = *(int*)temp;
			temp += sizeof(int);
			int serial = *(int*)temp;
			temp += sizeof(int);
			set->insert(QPair<int, int>(BlockNo, serial));
		}
		p += getUnitBytes();
	}
}

bool LeafBlock::lessThan(QString a, QString b) const
{
	bool ret;
	Attribute::DataType type = getDataType();
	if(type == Attribute::INT) {
		int x = a.toInt();
		int y = b.toInt();
		ret = x < y;
	} else if(type == Attribute::FLOAT) {
		double x = a.toDouble();
		double y = b.toDouble();
		ret = x < y;
	} else {
		ret = a < b;
	}
	return ret;
}

uint LeafBlock::getValueBytes() const
{
	uint ret;
	Attribute::DataType type = getDataType();
	if(type == Attribute::INT) {
		ret = sizeof(int);
	} else if(type == Attribute::FLOAT) {
		ret = sizeof(double);
	} else {
		ret = uint(type) + 1;
	}
	return ret;
}

QDebug operator<<(QDebug debug, const LeafBlock& block)
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
	debug << "prevNo:" << block.getPrevNo() << endl;
	debug << "nextNo:" << block.getNextNo() << endl;

	debug << "firstKey" << block.getFirstKey() << endl;

	Byte *p = block.valueZone;
	Attribute::DataType type = block.getDataType();
	for(int i=0; i<block.getUnitCount(); i++)
	{
		debug << i << "value:";
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
		debug << ", BlockNo:" << *(int*)p;
		p += sizeof(int);
		debug << ", serial:" << *(int*)p << endl;
		p += sizeof(int);
	}

	return debug;
}

QDebug operator<<(QDebug debug, const LeafBlock* block)
{
	return block? (debug << *block): debug << "NULL";
}
