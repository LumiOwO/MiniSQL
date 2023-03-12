#include "../blocks.h"
#include "../../table/table.h"
#include "../../myerror/myerror.h"
#include <fstream>
using namespace std;

TableBlock::Header::Header(pToData startAddr):
	count(*(int*)(startAddr)),
	attrNum(*(int*)(startAddr + sizeof(count))),
	recordBytes(*(uint*)(startAddr + sizeof(count) + sizeof(attrNum)))
{
	count = 0;
	attrNum = 0;
	recordBytes = 0;
}

TableBlock::TableBlock(int No, QString name):
	Block(ID, No, name),
	header((pToData)(dataZone + offsetToBegin()))
{
	attrZone = (pToData)dataZone + offsetToBegin() + header.bytes();
	recordZone = attrZone;

	modifyValidBytes(+int(header.bytes()));
}

TableBlock::TableBlock(QString name, QVector<Attribute*> attributes, int No):
	TableBlock(No, name)
{
	header.attrNum = attributes.size();

	pToData p = attrZone;
	QVectorIterator<Attribute*> iter(attributes);
	while(iter.hasNext())
	{
		Attribute* attribute = iter.next();
		attribute->write(p);
		p += attribute->bytes();

		header.recordBytes += attribute->valueBytes();
		modifyValidBytes(+int(attribute->bytes()));
		delete attribute;
	}
	recordZone = p;
}

TableBlock::TableBlock(pToData data):
	TableBlock(0)
{
	memcpy((pToData)dataZone, data, BLOCK_SIZE);
	recordZone = attrZone + uint(getAttrNum()) * Attribute::bytes();
}

QVector<Attribute*> TableBlock::getAttributes() const
{
	QVector<Attribute*> ret;
	pToData p = attrZone;
	for(int i=0; i<getAttrNum(); i++)
	{
		ret.push_back(new Attribute(p));
		p += Attribute::bytes();
	}
	return ret;
}

QVector<QStringList> TableBlock::getRecords() const
{
	QVector<QStringList> ret;
	pToData p = recordZone;

	QVector<Attribute*> attributes = getAttributes();
	QVectorIterator<Attribute*> iter(attributes);
	for(int i=0; i<getRecordCount(); i++)
	{
		QStringList record;
		iter.toFront();
		while(iter.hasNext())
		{
			Attribute::DataType type = iter.next()->getDataType();
			if(type == Attribute::FLOAT) {
				record << QString::number(*(double*)p);
				p += sizeof(double);
			} else if(type == Attribute::INT) {
				record << QString::number(*(int*)p);
				p += sizeof(int);
			} else {
				record << QString(p);
				p += type + 1;
			}
		}
		ret.push_back(record);
	}

	iter.toFront();
	while(iter.hasNext())
		delete iter.next();

	return ret;
}

int TableBlock::getAttrNo(QString attrName) const
{
	int ret = -1;
	for(int i=0; i<getAttrNum(); i++)
		if(getAttrName(i).toLower() == attrName.toLower())
		{
			ret = i;
			break;
		}
	return ret;
}

QString TableBlock::getAttrName(int i) const
{
	Byte *p = attrZone + uint(i) * Attribute::bytes();
	return Attribute(p).getName();
}

Attribute::DataType TableBlock::getDataType(int i) const
{
	Byte *p = attrZone + uint(i) * Attribute::bytes();
	return Attribute(p).getDataType();
}

bool TableBlock::isUniqueAt(int i) const
{
	Byte *p = attrZone + uint(i) * Attribute::bytes();
	return Attribute(p).isUnique();
}

bool TableBlock::canInsert(QStringList values) const
{
	bool ret = true;
	// check attribute number
	if(values.size() != getAttrNum())
		throw MyError::INSERT_ATTRINUM_ERROR;
	// check attribute type
	for(int i=0; i<getAttrNum(); i++)
	{
		Attribute::DataType type = getDataType(i);
		QString value = values[i];
		if(type == Attribute::FLOAT) {
			value.toDouble(&ret);
			if(!ret) throw MyError::INSERT_DATATYPE_ERROR;
		} else if(type == Attribute::INT) {
			value.toInt(&ret);
			if(!ret) throw MyError::INSERT_DATATYPE_ERROR;
		} else {
			ret = value.toStdString().length() <= uint(type);
			if(!ret) throw MyError::INSERT_STRING_TOOLONG;
		}

		// check uniqueness
		if(isUniqueAt(i))
		{
			// create equal condition
			Condition cond(getAttrName(i), value, Condition::EQUAL);
			cond.setTableName(getTableName());
			// find equal value
			QSet<QPair<int, int>>* set = cond.getSatisfied();
			bool duplicate = !set->isEmpty();
			delete set;

			if(duplicate) throw MyError::UNIQUE_DUPLICATE;
		}
	}

	return ret && (BLOCK_SIZE - offsetToEnd() >= getRecordBytes());
}

void TableBlock::insertRecord(QStringList values)
{
	setDirty(true);
	Byte *p = recordZone + uint(getRecordCount()) * getRecordBytes();
	for(int i=0; i<getAttrNum(); i++)
	{
		Attribute::DataType type = getDataType(i);
		QString value = values[i];
		if(type == Attribute::FLOAT) {
			double temp = value.toDouble();
			memcpy(p, &temp, sizeof(double));
			p += sizeof(double);
		} else if(type == Attribute::INT) {
			int temp = value.toInt();
			memcpy(p, &temp, sizeof(int));
			p += sizeof(int);
		} else {
			memcpy(p, value.toStdString().data(), uint(type) + 1);
			p += type + 1;
		}
	}
	header.count++;
	modifyValidBytes(+int(getRecordBytes()));
}

void TableBlock::dropRecords(QVector<int> recordsNo)
{
	int cnt = this->getRecordCount();
	int deleted = 0;
	for(int i=0; i<cnt; i++) {
		if(recordsNo.contains(i)) {
			setDirty(true);
			if(i != cnt - 1)
			{
				pToData p = recordZone + uint(i+1-deleted)*getRecordBytes();
				memcpy(p-getRecordBytes(), p, BLOCK_SIZE - uint(p - dataZone));
			}
			modifyValidBytes(-int(getRecordBytes()));
			header.count --;
			deleted ++;
		}
	}
}

void TableBlock::dropAllRecords()
{
	setDirty(true);
	modifyValidBytes(-int(getRecordBytes()) * getRecordCount());
	header.count = 0;
}

QDebug operator<<(QDebug debug, const TableBlock& block)
{
	debug << "block name: " << block.getName() << endl;
	debug << "block id: " << block.getID() << endl;
	debug << "block No: " << block.getNo() << endl;
	debug << "isDirty: " << block.isdirty() << endl;
	debug << "isPinned: " << block.isPinned() << endl;
	debug << "valid Bytes: " << block.offsetToEnd() << endl;

	debug << "record count: " << block.getRecordCount() << endl;
	debug << "attribute numbers: " << block.getAttrNum() << endl;
	debug << "record bytes: " << block.getRecordBytes() << endl;

	pToData p = block.attrZone;
	for(int i=0; i<block.getAttrNum(); i++)
	{
		debug << "Attr" << i << ":";
		debug << Attribute(p) << endl;
		p += Attribute::bytes();
	}

	p = block.recordZone;
	for(int i=0; i<block.getRecordCount(); i++)
	{
		debug << "record" << i << ": ";
		for(int j=0; j<block.getAttrNum(); j++)
		{
			Attribute::DataType type = block.getDataType(j);
			if(type == Attribute::FLOAT) {
				debug << *(double*)p;
				p += sizeof(double);
			} else if(type == Attribute::INT) {
				debug << *(int*)p;
				p += sizeof(int);
			} else {
				debug << p;
				p += type + 1;
			}
			debug << "\t";
		}
		debug << endl;
	}
	return debug;
}

QDebug operator<<(QDebug debug, const TableBlock* block)
{
	return block? (debug << *block): debug << "NULL";
}
