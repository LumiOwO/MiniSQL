#include "attribute.h"

Attribute::Attribute(const pToData data)
{
	pToData p = (pToData)data;
	// get name
	name = QString(p);
	p += Block::STRLEN_MAX;
	// get dataType
	memcpy(&dataType, p, sizeof(dataType));
	p += sizeof(dataType);
	// get isUnique
	memcpy(&unique, p, sizeof(unique));
	p += sizeof(unique);
}

void Attribute::write(const pToData dataZone)
{
	pToData p = (pToData)dataZone;
	// write name
	memcpy(p, name.toStdString().data(), Block::STRLEN_MAX);
	p += Block::STRLEN_MAX;
	// write dataType
	memcpy(p, &dataType, sizeof(dataType));
	p += sizeof(dataType);
	// write isUnique
	memcpy(p, &unique, sizeof(unique));
	p += sizeof(unique);
}

QDebug operator<<(QDebug debug, const Attribute& attribute)
{
	debug << attribute.getName().toStdString().data() << "\t";
	debug << attribute.getDataType() << "\t";
	debug << attribute.isUnique() << "\t";
	return debug;
}

QDebug operator<<(QDebug debug, const Attribute* attribute)
{
	return debug << *attribute;
}
