#include "../blocks.h"

NodeBlock::NodeBlock(char id, int No, QString name):
	Block(id, No, name)
{

}

NodeBlock* NodeBlock::getBlock(const QVector<NodeBlock *> &nodes, int BlockNo)
{
	NodeBlock* ret = nullptr;
	QVectorIterator<NodeBlock*> iter(nodes);
	while(iter.hasNext())
	{
		NodeBlock* temp = iter.next();
		if(temp->getNo() == BlockNo)
		{
			ret = temp;
			break;
		}
	}
	return ret;
}

QDebug operator<<(QDebug debug, const NodeBlock& block)
{
	return debug << &block;
}

QDebug operator<<(QDebug debug, const NodeBlock* block)
{
	return debug << (Block*)block;
}
