#include "../blocks.h"
#include "../../managers/managers.h"
#include <fstream>
using namespace std;

CatalogBlock::Header::Header(pToData dataAddr):
	count(*(int*)(dataAddr))
{
	count = 0;
}

CatalogBlock::CatalogBlock(int No):
	Block(ID, No, CatalogBlock::NAME),
	header((pToData)dataZone + offsetToBegin()),
	tableZone((pToData)dataZone + offsetToBegin() + header.bytes())
{
	modifyValidBytes(+int(header.bytes()));
}

CatalogBlock::CatalogBlock(pToData data):
	CatalogBlock(0)
{
	memcpy((pToData)dataZone, data, BLOCK_SIZE);
}

void CatalogBlock::insertTable(QString name)
{
	setDirty(true);
	Byte *p = tableZone + getTableCount() * STRLEN_MAX;
	memcpy(p, name.toLower().toStdString().data(), STRLEN_MAX);
	header.count++;
	modifyValidBytes(+STRLEN_MAX);
}

void CatalogBlock::dropTable(QString name)
{
	setDirty(true);
	Byte *p = tableZone;
	for(; p != name; p += STRLEN_MAX)
		;
	memcpy(p, p+STRLEN_MAX, offsetToEnd() - uint(p+STRLEN_MAX - dataZone));

	header.count--;
	modifyValidBytes(-STRLEN_MAX);

	if(getTableCount() == 0)
		BufferManager::getInstance()->removeBlock(this);
}

bool CatalogBlock::hasTable(QString name) const
{
	name = name.toLower();
	bool ret = false;

	Byte* p = tableZone;
	for(int i=0; i<getTableCount(); i++) {
		if(name == p) {
			ret = true;
			break;
		}
		p += STRLEN_MAX;
	}
	return ret;
}

QDebug operator<<(QDebug debug, const CatalogBlock& block)
{
	debug << "block name: " << block.getName() << endl;
	debug << "block id: " << block.getID() << endl;
	debug << "block No: " << block.getNo() << endl;
	debug << "isDirty: " << block.isdirty() << endl;
	debug << "isPinned: " << block.isPinned() << endl;
	debug << "valid Bytes: " << block.offsetToEnd() << endl;

	debug << "table count: " << block.getTableCount() << endl;

	Byte *p = block.tableZone;
	for(int i=0; i<block.getTableCount(); i++)
	{
		debug << i << ":" << p << endl;
		p += CatalogBlock::STRLEN_MAX;
	}

	return debug;
}

QDebug operator<<(QDebug debug, const CatalogBlock* block)
{
	return block? (debug << *block): debug << "NULL";
}
