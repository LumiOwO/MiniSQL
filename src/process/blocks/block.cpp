#include "blocks.h"

Block::Header::Header(const CStringRef startAttr):
	name(startAttr),
	id(*(char*)(startAttr + STRLEN_MAX)),
	No(*(int*)(startAttr + STRLEN_MAX + sizeof(id))),
	validBytes(*(uint*)(startAttr + STRLEN_MAX + sizeof(id) + sizeof(No)))
{

}

Block::Block(char id, int No, QString name):
	dataZone(new Byte[BLOCK_SIZE]),
	dirty(false), pinned(false),
	header(dataZone)
{
	memcpy((char*)header.name, name.toLower().toStdString().data(), STRLEN_MAX);
	header.id = id;
	header.No = No;
	header.validBytes = header.bytes();
}

Block::~Block()
{
	delete[] dataZone;
}


QDebug operator<<(QDebug debug, const Block& block)
{
	return debug << &block;
}

QDebug operator<<(QDebug debug, const Block* block)
{
	if(!block)
		debug << "NULL";
	else if(block->getID() == CatalogBlock::ID)
		debug << (CatalogBlock*)block;
	else if(block->getID() == TableBlock::ID)
		debug << (TableBlock*)block;
	else if(block->getID() == IndexCatalogBlock::ID)
		debug << (IndexCatalogBlock*)block;
	else if(block->getID() == LeafBlock::ID)
		debug << (LeafBlock*)block;
	else if(block->getID() == NonLeafBlock::ID)
		debug << (NonLeafBlock*)block;
	return debug;
}
