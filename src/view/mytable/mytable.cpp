#include "mytable.h"

MyTable::MyTable():
	table(new QTableWidget)
{
	// set style
	table->horizontalHeader()->setStyleSheet(
		"QHeaderView::section "
		"{"
			"color: rgb(255, 255, 255);"
			"background-color: rgb(55, 55, 55);"
			"border: none;"
			"height: 32px;"
		"}"
	);
	table->verticalHeader()->setStyleSheet(
		"QHeaderView::section "
		"{"
			"color: rgb(255, 255, 255);"
			"background-color: rgb(55, 55, 55);"
			"border: none;"
			"width: 40px;"
		"}"
	);
	table->setStyleSheet(
		"QTableCornerButton::section "
		"{"
			"background-color: rgb(55, 55, 55); "
		"}"
		"QTableWidget"
		"{"
			"color: rgb(235, 235, 235);"
			"background-color: rgb(64, 66, 68);"
			"selection-color: rgb(0, 0, 0);"
			"selection-background-color: rgb(202, 229, 254);"
		"}"
	);

	table->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
	table->horizontalHeader()->setStretchLastSection(true);
	table->verticalHeader()->setStretchLastSection(true);
	table->setFocusPolicy(Qt::NoFocus);
	table->verticalHeader()->setDefaultAlignment(Qt::AlignCenter);
	table->setFrameShape(QFrame::NoFrame);
	table->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

MyTable::~MyTable()
{
	delete table;
}

QTableWidget *MyTable::getTable()
{
	return table;
}

void MyTable::clearTable()
{
	table->setRowCount(0);
	table->setColumnCount(0);
	table->repaint();
	table->viewport()->repaint();
	table->verticalHeader()->repaint();
	table->verticalHeader()->viewport()->repaint();
}

void MyTable::setGridValue(int row, int column, QString value)
{
	table->setItem(row, column, new QTableWidgetItem(value));
	table->item(row, column)->setTextAlignment(Qt::AlignCenter);
}

void MyTable::update(QStringList attributes, QVector<QStringList> records)
{
	// set table size
	int columns = attributes.size() == 0? attributes.size(): attributes.size() + 1;
	int rows = records.size() == 0? records.size(): records.size() + 1;
	table->setColumnCount(columns);
	table->setRowCount(rows);

	// set table value
	QStringList lines;
	for(int i=0; i<records.size(); i++)
		for(int j=0; j<records[i].size(); j++)
			setGridValue(i, j, records[i][j]);

	// set empty parts
	for(int i=0; i<columns; i++) {
		QTableWidgetItem* item = new QTableWidgetItem("");
		item->setFlags(item->flags() & (~Qt::ItemIsSelectable));
		table->setItem(rows-1, i, item);
	}
	for(int i=0; i<rows-1; i++) {
		QTableWidgetItem* item = new QTableWidgetItem("");
		item->setFlags(item->flags() & (~Qt::ItemIsSelectable));
		table->setItem(i, columns-1, item);
		lines << QString::number(i+1);
	}
	// set header
	table->setHorizontalHeaderLabels(attributes << "");
	table->setVerticalHeaderLabels(lines << "");

}




