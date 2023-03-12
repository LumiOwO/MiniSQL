#include "BPlusTree.h"
#include "../managers/managers.h"

BPlusTree::BPlusTree(NodeBlock* root):
	root(root)
{
	this->valueType = root->getDataType();
}

QSet<QPair<int, int>>* BPlusTree::getSatisfiedSet(const Condition *cond) const
{
	BufferManager *bufferManager = BufferManager::getInstance();
	QSet<QPair<int, int>>* ret = new QSet<QPair<int, int>>();

	QString indexName = root->getName();
	QString value = cond->getValue();

	NodeBlock* p = root;
	while(p->getID() != LeafBlock::ID)
	{
		NonLeafBlock* nonLeaf = (NonLeafBlock*)p;
		p = (NonLeafBlock*)bufferManager->getBlock(indexName, nonLeaf->getChildNo(value));
	}

	LeafBlock* leaf = (LeafBlock*)p;
	LeafBlock* iter = leaf;
	if(cond->getCompare() == Condition::EQUAL) {
		leaf->pushEqual(ret, value);
	} else if(cond->getCompare() == Condition::NOT_EQUAL) {
		leaf->pushNotEqual(ret, value);
		while(iter->hasPrev()) {
			iter = (LeafBlock*)bufferManager->getBlock(indexName, iter->getPrevNo());
			iter->pushAll(ret);
		}
		iter = leaf;
		while(iter->hasNext()) {
			iter = (LeafBlock*)bufferManager->getBlock(indexName, iter->getNextNo());
			iter->pushAll(ret);
		}
	} else if(cond->getCompare() == Condition::LESS_THAN) {
		leaf->pushLessThan(ret, value);
		while(iter->hasPrev()) {
			iter = (LeafBlock*)bufferManager->getBlock(indexName, iter->getPrevNo());
			iter->pushAll(ret);
		}
	} else if(cond->getCompare() == Condition::LARGER_THAN) {
		leaf->pushLargerThan(ret, value);
		while(iter->hasNext()) {
			iter = (LeafBlock*)bufferManager->getBlock(indexName, iter->getNextNo());
			iter->pushAll(ret);
		}
	} else if(cond->getCompare() == Condition::NOT_LESS_THAN) {
		leaf->pushNotLessThan(ret, value);
		while(iter->hasNext()) {
			iter = (LeafBlock*)bufferManager->getBlock(indexName, iter->getNextNo());
			iter->pushAll(ret);
		}
	} else if(cond->getCompare() == Condition::NOT_LARGER_THAN) {
		leaf->pushNotLargerThan(ret, value);
		while(iter->hasPrev()) {
			iter = (LeafBlock*)bufferManager->getBlock(indexName, iter->getPrevNo());
			iter->pushAll(ret);
		}
	}

	return ret;
}

QPair<int, int> BPlusTree::find(QString value, const QVector<NodeBlock *> nodes)
{
	NodeBlock* p = root;
	while(p->getID() != LeafBlock::ID)
	{
		NonLeafBlock* nonLeaf = (NonLeafBlock*)p;
		p = NodeBlock::getBlock(nodes, nonLeaf->getChildNo(value));
	}

	int blockNo = p->getNo();
	int serial = ((LeafBlock*)p)->getSerial(value);
	return QPair<int, int>(blockNo, serial);
}

void BPlusTree::insert(QVector<QPair<QString, QPair<int, int>>> values)
{
	BufferManager *bufferManager = BufferManager::getInstance();
	QString indexName = root->getName();
	int num = bufferManager->getBlockNums(indexName);

	// get all blocks
	QVector<NodeBlock*> nodes;
	if(num == 0)
		nodes << root;
	else for(int i=0; i<num; i++)
	{
		NodeBlock* block = (NodeBlock*)bufferManager->getBlock(indexName, i);
		bufferManager->DequeueBlock(block);
		block->setDirty(true);
		nodes << block;
	}
	// remove file
	bufferManager->deleteFile(indexName);

	// insert key values
	for(int i=0; i<values.size(); i++)
	{
		QString keyValue = values[i].first;
		int blockNo = values[i].second.first;
		int serial = values[i].second.second;
//		qDebug() << "insert" << i << "times:";
//		qDebug() << keyValue << blockNo << serial;
		insert(nodes, keyValue, blockNo, serial);
	}

	// store blocks
	for(int i=0; i<nodes.size(); i++)
	{
		nodes[i]->setNo(i);
		bufferManager->EnqueueBlock(nodes[i]);
	}
}

void BPlusTree::insert(QVector<NodeBlock *>& nodes, QString value, int BlockNo, int serial)
{
	int position = find(value, nodes).first;
	LeafBlock* block = (LeafBlock*)nodes[position];
//	qDebug() << block;
	int prevSize = nodes.size();
	block->insert(nodes, value, BlockNo, serial);

	// find new root
	if(nodes.size() != prevSize)
	{
		QVectorIterator<NodeBlock*> iter(nodes);
		iter.toBack();
		while(iter.hasPrevious()) {
			NodeBlock* block = iter.previous();
			if(block->getParentNo() < 0) {
				root = block;
				break;
			}
		}
	}
}

void BPlusTree::deLete(QStringList values)
{
	BufferManager *bufferManager = BufferManager::getInstance();
	QString indexName = root->getName();
	int num = bufferManager->getBlockNums(indexName);

	// get all blocks
	QVector<NodeBlock*> nodes;
	if(num == 0)
		nodes << root;
	else for(int i=0; i<num; i++)
	{
		NodeBlock* block = (NodeBlock*)bufferManager->getBlock(indexName, i);
		bufferManager->DequeueBlock(block);
		block->setDirty(true);
		nodes << block;
	}
	// remove file
	bufferManager->deleteFile(indexName);

	// delete key values
	for(int i=0; i<values.size(); i++)
	{
		QString keyValue = values[i];
		deLete(nodes, keyValue);
	}

	// store blocks
	for(int i=0; i<nodes.size(); i++)
	{
		nodes[i]->setNo(i);
		bufferManager->EnqueueBlock(nodes[i]);
	}
}

void BPlusTree::deLeteAll()
{
	BufferManager *bufferManager = BufferManager::getInstance();
	QString indexName = root->getName();
	Attribute::DataType type = root->getDataType();
	int num = bufferManager->getBlockNums(indexName);

	// delete all values
	QVector<NodeBlock*> nodes;
	for(int i=0; i<num; i++)
	{
		NodeBlock* block = (NodeBlock*)bufferManager->getBlock(indexName, i);
		bufferManager->DequeueBlock(block);
		delete block;
	}
	// remove file
	bufferManager->deleteFile(indexName);

	// create new root
	root = new LeafBlock(indexName, 0, type);
	bufferManager->EnqueueBlock(root);

}

void BPlusTree::deLete(QVector<NodeBlock *> &nodes, QString keyValue)
{
	QPair<int, int> position = find(keyValue, nodes);
	LeafBlock* block = (LeafBlock*)nodes[position.first];
//	qDebug() << block;
	int prevSize = nodes.size();
	block->deLete(nodes, position.second);

	// find new root
	if(nodes.size() != prevSize)
	{
		QVectorIterator<NodeBlock*> iter(nodes);
		iter.toBack();
		while(iter.hasPrevious()) {
			NodeBlock* block = iter.previous();
			if(block->getParentNo() < 0) {
				root = block;
				break;
			}
		}
	}
}

