#ifndef ATTRIBUTE_H_
#define ATTRIBUTE_H_

#include "../blocks/block.h"
#include <QString>

class Attribute
{
public:
	typedef int DataType;
	static const DataType FLOAT = -1;
	static const DataType INT = 0;
private:
	QString name;
	DataType dataType;
	bool unique;
public:
	Attribute(QString name, DataType dataType, bool unique):
		name(name), dataType(dataType), unique(unique) {}
	Attribute(const pToData data);

	// getter
	QString getName() const { return name; }
	DataType getDataType() const { return dataType; }
	bool isUnique() const { return unique; }
	// setter
	void setUnique(bool unique) { this->unique = unique; }

	// write data
	void write(const pToData dataZone);

	// get the bytes of the attribute itself
	static uint bytes() { return Block::STRLEN_MAX + sizeof(DataType) + sizeof(bool); }
	// get the bytes of the relative record
	uint valueBytes() {
		return  dataType == FLOAT?  sizeof(double):
				dataType == INT?	sizeof(int):
									uint(dataType) + 1;
	}

	// debug
	friend QDebug operator<<(QDebug debug, const Attribute& attribute);
	friend QDebug operator<<(QDebug debug, const Attribute* attribute);
};

#endif
