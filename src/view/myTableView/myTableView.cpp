#include "myTableView.h"
#include "../../process/api/api.h"
#include <QHeaderView>

MyTableView::MyTableView():
	tableView(new QTreeView()),
	catalog(new QStandardItem("Tables")),
	indexCatalog(new QStandardItem("Index"))
{
	tableView->header()->hide();
	catalog->setEditable(false);

	QStandardItemModel *model = new QStandardItemModel();
	model->appendRow(catalog);
	model->appendRow(indexCatalog);
	tableView->setModel(model);
}

MyTableView::~MyTableView()
{
	delete catalog;
	delete tableView;

}

void MyTableView::updateTableView()
{
	API *api = API::getInstance();

	// update catalog view
	catalog->removeRows(0, catalog->rowCount());
	QStringList tables = api->getAllTables();
	for(int i=0; i<tables.size(); i++)
	{
		QString tableName = tables[i];
		QStandardItem *temp = new QStandardItem(tableName);
		// get all attribute of the table
		QStringList attrNames = api->getAttrNames(tableName);
		QStringListIterator iter(attrNames);
		while(iter.hasNext())
			temp->appendRow(new QStandardItem(iter.next()));

		temp->setEditable(false);
		catalog->appendRow(temp);
	}
	// update index view
	indexCatalog->removeRows(0, indexCatalog->rowCount());
	QVector<QStringList> indexInfo = api->getAllIndexInfo();
	for(int i=0; i<indexInfo.size(); i++)
	{
		QStringList index = indexInfo[i];
		QStandardItem *temp = new QStandardItem(index[0]);

		QString attrInfo = index[1] + " (" + index[2] + ")";
		temp->appendRow(new QStandardItem(attrInfo));

		temp->setEditable(false);
		indexCatalog->appendRow(temp);
	}

	// expand the view
	tableView->setExpanded(catalog->index(), true);
	tableView->setExpanded(indexCatalog->index(), true);
}
