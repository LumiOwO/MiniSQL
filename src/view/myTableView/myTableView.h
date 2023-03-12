#ifndef _MYTABLEVIEW_H_
#define _MYTABLEVIEW_H_

#include <QTreeView>
#include <QStandardItemModel>

class MyTableView
{
private:
	QTreeView *tableView;
	QStandardItem *catalog;
	QStandardItem *indexCatalog;
public:
	MyTableView();
	~MyTableView();

	QTreeView* getWidget() { return tableView; }
	// show given tables
	void updateTableView();
};

#endif
