#ifndef MYTABLE_H
#define MYTABLE_H

#include <QTableWidget>
#include <QHeaderView>
#include <QVector>

class MyTable
{
private:
	QTableWidget *table;
public:
	MyTable();
	~MyTable();

	void clearTable();
	QTableWidget *getTable();
	void update(QStringList attributes, QVector<QStringList> records);
private:
	void setGridValue(int row, int column, QString value);
};

#endif
