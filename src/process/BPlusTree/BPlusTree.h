#ifndef _BPLUSTREE_H_
#define _BPLUSTREE_H_

#include "../blocks/blocks.h"
#include "../attribute/attribute.h"
#include "../condition/condition.h"

class BPlusTree
{
private:
	NodeBlock* root;
	Attribute::DataType valueType;
public:
	BPlusTree(NodeBlock* root);

	void insert(QVector<QPair<QString, QPair<int, int>>> values);
	void deLete(QStringList values);
	void deLeteAll();

	QSet<QPair<int, int>>* getSatisfiedSet(const Condition* cond) const;
private:
	QPair<int, int> find(QString value, const QVector<NodeBlock*> nodes);
	void insert(QVector<NodeBlock *> &nodes, QString value, int BlockNo, int serial);
	void deLete(QVector<NodeBlock *> &nodes, QString keyValue);
};

#endif
