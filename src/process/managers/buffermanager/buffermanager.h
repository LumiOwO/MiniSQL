#ifndef BUFFERMANAGER_H_
#define BUFFERMANAGER_H_

#include <QQueue>
#include "../../blocks/block.h"

class BufferManager
{
// Singleton
private:
	BufferManager() {}
	static BufferManager *instance;
public:
	static BufferManager *getInstance();
	static void free();

public:
	static const int MAX_PAGES = 20;
	static constexpr const CString DB_DIR = "./db/";
private:
	QQueue<Block*> pages;

public:
	// get block in buffer pages
	Block* getBlock(QString name, int No);
	// get the number of blocks in the file
	int getBlockNums(QString name);
	// load block from file
	Block* loadBlock(QString name, int No);
	// store block to file
	void storeBlock(Block* block);
	// enqueue block into pages
	void EnqueueBlock(Block* block);
	// dequeue block from pages
	void DequeueBlock(Block* block);
	// remove empty block in the pages and file
	void removeBlock(Block* block);

	// delete file by given name
	void deleteFile(QString name);

	// update the full pages to enqueue new block
	void updatePages();
	// write all pages to file
	void storePages();
};


#endif
