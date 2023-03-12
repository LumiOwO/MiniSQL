#include "../managers.h"
#include "../../myerror/myerror.h"
#include "../../blocks/blocks.h"
#include <QFile>
#include <fstream>
using namespace std;

BufferManager *BufferManager::instance = nullptr;

BufferManager *BufferManager::getInstance()
{
	if(!instance)
	{
		instance = new BufferManager();
	}
	return instance;
}

void BufferManager::free()
{
	if(instance) {
		while(!instance->pages.isEmpty())
			delete instance->pages.dequeue();

		delete instance;
		instance = nullptr;
	}
}

Block* BufferManager::getBlock(QString name, int No)
{
	Block* ret = nullptr;
	if(No >= 0) {
		name = name.toLower();

		QListIterator<Block*> iter(pages);
		iter.toBack();
		while(iter.hasPrevious())
		{
			Block* block = iter.previous();
			if(block->getName() == name && block->getNo() == No) {
				ret = block;
				break;
			}
		}
		if(!ret)
			ret = loadBlock(name, No);
	}
	return ret;
}

int BufferManager::getBlockNums(QString name)
{
	name = name.toLower();
	int max = 0;
	QListIterator<Block*> iter(pages);
	while(iter.hasNext())
	{
		Block* block = iter.next();
		if(block->getName() == name && block->getNo() + 1 > max)
			max = block->getNo() + 1;
	}

	QFile file(DB_DIR + name);
	if(file.exists()) {
		int temp = int(file.size() / Block::BLOCK_SIZE);
		max = temp > max? temp: max;
	}
	return max;
}

Block* BufferManager::loadBlock(QString name, int No)
{
	name = name.toLower();
	Block* block = nullptr;
	// open the file
	fstream in(DB_DIR + name.toStdString(), ios::in|ios::binary);

	if(in) {
		// read block data
		pToData data = new Byte[Block::BLOCK_SIZE];
		in.seekg(No * Block::BLOCK_SIZE);
		in.read(data, Block::BLOCK_SIZE);

		// create new block
		Block* test = new CatalogBlock(data);
		if(test->getID() == CatalogBlock::ID)
			block = new CatalogBlock(data);
		else if(test->getID() == LeafBlock::ID)
			block = new LeafBlock(data);
		else if(test->getID() == NonLeafBlock::ID)
			block = new NonLeafBlock(data);
		else if(test->getID() == TableBlock::ID)
			block = new TableBlock(data);
		else if(test->getID() == IndexCatalogBlock::ID)
			block = new IndexCatalogBlock(data);
		delete test;

		EnqueueBlock(block);

		delete[] data;
		in.close();
	}
	return block;
}

void BufferManager::storeBlock(Block *block)
{
	string filename = DB_DIR + block->getName().toStdString();
	fstream out(filename, ios::in | ios::out | ios::binary);
	if(!out)
		out = fstream(filename, ios::out | ios::binary);
	const pToData p = block->getDataZone();
	out.seekp(block->getNo() * Block::BLOCK_SIZE);
	out.write(p, Block::BLOCK_SIZE);
	out.close();
}

void BufferManager::EnqueueBlock(Block *block)
{
	if(pages.size() == MAX_PAGES)
		updatePages();
	pages.enqueue(block);
}

void BufferManager::DequeueBlock(Block *block)
{
	pages.removeOne(block);
}

void BufferManager::removeBlock(Block *block)
{
	pages.removeOne(block);
	QString filename = DB_DIR + block->getName();
	QString tempname = DB_DIR + block->getName() + "_temp";
	int No = block->getNo();
	int num = getBlockNums(block->getName());
	delete block;

	fstream in(filename.toStdString(), ios::in | ios::binary);
	if(!in)
		in.close();
	else {
		pToData buffer = new Byte[Block::BLOCK_SIZE];
		fstream out(tempname.toStdString(), ios::out | ios::binary);
		for(int i=0; i<num; i++)
		{
			if(i != No) {
				in.read(buffer, Block::BLOCK_SIZE);
				out.write(buffer, Block::BLOCK_SIZE);
			} else {
				in.seekg(Block::BLOCK_SIZE, ios::cur);
			}
		}
		out.close();
		in.close();
		delete[] buffer;

		QFile::remove(filename);
		QFile tempFile(tempname);
		if(tempFile.size() > 0)
			tempFile.rename(filename);
		else
			tempFile.remove();
	}
}

void BufferManager::deleteFile(QString name)
{
	QFile::remove(DB_DIR + name);
}

void BufferManager::storePages()
{
	QListIterator<Block*> iter(pages);
	while(iter.hasNext())
	{
		Block* block = iter.next();
		if(block->isdirty())
			storeBlock(block);
		block->setDirty(false);
	}
}

void BufferManager::updatePages()
{
	for(int i=0; i<pages.size(); i++) {
		if(!pages[i]->isdirty()) {
			Block* block = pages[i];
			pages.removeAt(i--);
			delete block;
		}
	}
	if(pages.size() == MAX_PAGES) {
		storePages();
		while(!instance->pages.isEmpty())
			delete instance->pages.dequeue();
	}
}
