#include "../blocks.h"
#include "../../managers/managers.h"

IndexCatalogBlock::Header::Header(pToData dataAddr):
	count(*(int*)(dataAddr))
{
	count = 0;
}

IndexCatalogBlock::IndexCatalogBlock(int No):
	Block(ID, No, IndexCatalogBlock::NAME),
	header((pToData)dataZone + offsetToBegin()),
	indexZone((pToData)dataZone + offsetToBegin() + header.bytes())
{
	modifyValidBytes(+int(header.bytes()));
}

IndexCatalogBlock::IndexCatalogBlock(pToData data):
	IndexCatalogBlock(0)
{
	memcpy((pToData)dataZone, data, BLOCK_SIZE);
}

void IndexCatalogBlock::insertIndex(QString indexName, QString tableName, QString attrName)
{
	setDirty(true);
	Byte *p = indexZone + getIndexCount() * STRLEN_MAX * 3;
	// write index name
	memcpy(p, indexName.toLower().toStdString().data(), STRLEN_MAX);
	p += STRLEN_MAX;
	// write table name
	memcpy(p, tableName.toLower().toStdString().data(), STRLEN_MAX);
	p += STRLEN_MAX;
	// write attribute name
	memcpy(p, attrName.toLower().toStdString().data(), STRLEN_MAX);
	p += STRLEN_MAX;

	header.count++;
	modifyValidBytes(+STRLEN_MAX*3);
}

void IndexCatalogBlock::dropIndex(QString indexName)
{
	setDirty(true);
	Byte *p = indexZone;
	for(; p != indexName; p += STRLEN_MAX * 3)
		;
	memcpy(p, p+STRLEN_MAX*3, offsetToEnd() - uint(p+STRLEN_MAX*3 - dataZone));

	header.count--;
	modifyValidBytes(-STRLEN_MAX*3);

	if(getIndexCount() == 0)
		BufferManager::getInstance()->removeBlock(this);
}

QString IndexCatalogBlock::getIndexName(QString tableName, QString attrName)
{
	tableName = tableName.toLower();
	attrName = attrName.toLower();
	QString ret;

	Byte* p = indexZone;
	for(int i=0; i<getIndexCount(); i++) {
		if(tableName == p + STRLEN_MAX && attrName == p + STRLEN_MAX*2) {
			ret = p;
			break;
		}
		p += STRLEN_MAX * 3;
	}
	return ret;
}

bool IndexCatalogBlock::hasIndex(QString indexName) const
{
	indexName = indexName.toLower();
	bool ret = false;

	Byte* p = indexZone;
	for(int i=0; i<getIndexCount(); i++) {
		if(indexName == p) {
			ret = true;
			break;
		}
		p += STRLEN_MAX * 3;
	}
	return ret;
}

bool IndexCatalogBlock::hasIndex(QString tableName, QString attrName) const
{
	tableName = tableName.toLower();
	attrName = attrName.toLower();
	bool ret = false;

	Byte* p = indexZone;
	for(int i=0; i<getIndexCount(); i++) {
		if(tableName == p + STRLEN_MAX && attrName == p + STRLEN_MAX*2) {
			ret = true;
			break;
		}
		p += STRLEN_MAX * 3;
	}
	return ret;
}

QDebug operator<<(QDebug debug, const IndexCatalogBlock& block)
{
	debug << "block name: " << block.getName() << endl;
	debug << "block id: " << block.getID() << endl;
	debug << "block No: " << block.getNo() << endl;
	debug << "isDirty: " << block.isdirty() << endl;
	debug << "isPinned: " << block.isPinned() << endl;
	debug << "valid Bytes: " << block.offsetToEnd() << endl;

	debug << "index count: " << block.getIndexCount() << endl;

	Byte *p = block.indexZone;
	for(int i=0; i<block.getIndexCount(); i++)
	{
		debug << i << "->";
		debug << "index name:" << p;
		p += CatalogBlock::STRLEN_MAX;
		debug << ", table name:" << p;
		p += CatalogBlock::STRLEN_MAX;
		debug << ", attribute name:" << p << endl;
		p += CatalogBlock::STRLEN_MAX;
	}

	return debug;
}

QDebug operator<<(QDebug debug, const IndexCatalogBlock* block)
{
	return block? (debug << *block): debug << "NULL";
}
